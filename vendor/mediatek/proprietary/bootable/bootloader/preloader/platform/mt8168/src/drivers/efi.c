#include "typedefs.h"
#include "print.h"
#include "blkdev.h"
#include "dram_buffer.h"
#include "partition.h"

typedef struct {
    u8 b[16];
} __attribute__((packed)) efi_guid_t;


typedef struct {
    u64 signature;
    u32 revision;
    u32 header_size;
    u32 header_crc32;
    u32 reserved;
    u64 my_lba;
    u64 alternate_lba;
    u64 first_usable_lba;
    u64 last_usable_lba;
    efi_guid_t disk_guid;
    u64 partition_entry_lba;
    u32 num_partition_entries;
    u32 sizeof_partition_entry;
    u32 partition_entry_array_crc32;
} __attribute__((packed)) gpt_header;

#define GPT_ENTRY_NAME_LEN  (72 / sizeof(u16))

typedef struct {
    efi_guid_t partition_type_guid;
    efi_guid_t unique_partition_guid;
    u64 starting_lba;
    u64 ending_lba;
    u64 attributes;
    u16 partition_name[GPT_ENTRY_NAME_LEN];
} __attribute__((packed))gpt_entry;

static part_t *part_ptr = NULL;

/*
 ********** Definition of Debug Macro **********
 */
#define TAG "[GPT_PL]"

#define LEVEL_ERR   (0x0001)
#define LEVEL_INFO  (0x0004)

#define DEBUG_LEVEL (LEVEL_ERR | LEVEL_INFO)

#define efi_err(fmt, args...)   \
do {    \
    if (DEBUG_LEVEL & LEVEL_ERR) {  \
        printf(fmt, ##args); \
    }   \
} while (0)

#define efi_info(fmt, args...)  \
do {    \
    if (DEBUG_LEVEL & LEVEL_INFO) {  \
        printf(fmt, ##args);    \
    }   \
} while (0)


/*
 ********** Definition of GPT buffer **********
 */
#define pgpt_header g_dram_buf->pgpt_header_buf
#define pgpt_entries g_dram_buf->pgpt_entries_buf

#define sgpt_header g_dram_buf->sgpt_header_buf
#define sgpt_entries g_dram_buf->sgpt_entries_buf

/*
 ********** Definition of CRC32 Calculation **********
 */
static int crc32_table_init = 0;
#define crc32_table g_dram_buf->crc32_table

/*
 *********** For Early GPT Parsing (SRAM) *********
 */
#define GPT_SRAM_BUF_SIZE   1024
unsigned char __NOBITS_SECTION__(.gpt_sram_buf) gpt_sram_buf[GPT_SRAM_BUF_SIZE];
u32 __NOBITS_SECTION__(.gpt_sram_crc32_tbl) gpt_sram_crc32_tbl[256];
part_t __NOBITS_SECTION__(.gpt_sram_part_info) gpt_sram_part_info;
struct part_meta_info __NOBITS_SECTION__(.gpt_sram_part_meta_info) gpt_sram_part_meta_info;
static gpt_header __NOBITS_SECTION__(.gpt_header_buf) gpt_header_buf;

struct part_info_t {
    u64 addr;
    u8 active;
};

static void early_init_crc32_table(u32* crc32_tbl)
{
    int i, j;
    u32 crc;
    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32_tbl[i] = crc;
    }
}

static u32 early_crc32(u32 crc, u8 *p, u32 len, u32* crc32_tbl)
{
    while (len--) {
        crc ^= *p++;
        crc = (crc >> 8) ^ crc32_tbl[crc & 255];
    }

    return crc;
}

static u32 early_efi_crc32(u32 crc, u8 *p, u32 len, u32* crc32_tbl)
{
	return early_crc32(crc, p, len, crc32_tbl);
}

static u32 early_efi_crc32_finalize(u32 crc)
{
	return crc ^ ~0L;
}

static void init_crc32_table(void)
{
    int i, j;
    u32 crc;

    if (crc32_table_init) {
        return;
    }

    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_init = 1;
}

static u32 crc32(u32 crc, u8 *p, u32 len)
{
    init_crc32_table();

    while (len--) {
        crc ^= *p++;
        crc = (crc >> 8) ^ crc32_table[crc & 255];
    }

    return crc;
}

static u32 efi_crc32(u8 *p, u32 len)
{
    return (crc32(~0L, p, len) ^ ~0L);
}


static void w2s(u8 *dst, int dst_max, u16 *src, int src_max)
{
    int i = 0;
    int len = min(src_max, dst_max - 1);

    while (i < len) {
        if (!src[i]) {
            break;
        }
        dst[i] = src[i] & 0xFF;
        i++;
    }

    dst[i] = 0;

    return;
}

extern u64 g_emmc_user_size;

static u64 last_lba(u32 part_id)
{
    /* Only support USER region now */
    return g_emmc_user_size / 512 - 1;
}

static int read_data(u8 *buf, u32 part_id, u64 lba, u64 size)
{
    int err;
    blkdev_t *dev;

    dev = blkdev_get(BOOTDEV_SDMMC);
    if (!dev) {
        efi_err("%sread data, err(no dev)\n", TAG);
        return 1;
    }

    err = dev->bread(dev, (u32)lba, (u32)(size / 512), buf, part_id);
    if (err) {
        efi_err("%sread data, err(%d)\n", TAG, err);
        return err;
    }

    return 0;
}


#define GPT_HEADER_SIGNATURE    0x5452415020494645ULL


static int parse_gpt_header(u32 part_id, u64 header_lba, u8 *header_buf, u8 *entries_buf)
{
    int i;

    int err;
    u32 calc_crc, orig_header_crc;
    u64 entries_real_size, entries_read_size;

    gpt_header *header = (gpt_header *)header_buf;
    gpt_entry *entries = (gpt_entry *)entries_buf;

    err = read_data(header_buf, part_id, header_lba, 512);
    if (err) {
        efi_err("%sread header(part_id=%d,lba=%llx), err(%d)\n",
                TAG, part_id, header_lba, err);
        return err;
    }

    if (header->signature != GPT_HEADER_SIGNATURE) {
        efi_err("%scheck header, err(signature 0x%llx!=0x%llx)\n",
                TAG, header->signature, GPT_HEADER_SIGNATURE);
        return 1;
    }

    orig_header_crc = header->header_crc32;
    header->header_crc32 = 0;
    calc_crc = efi_crc32((u8 *)header, header->header_size);

    if (orig_header_crc != calc_crc) {
        efi_err("%scheck header, err(crc 0x%x!=0x%x(calc))\n",
                TAG, orig_header_crc, calc_crc);
        return 1;
    }

    header->header_crc32 = orig_header_crc;

    if (header->my_lba != header_lba) {
        efi_err("%scheck header, err(my_lba 0x%llx!=0x%llx)\n",
                TAG, header->my_lba, header_lba);
        return 1;
    }

    ASSERT(header->num_partition_entries <= 128); // max partition count in MBR is 128
    ASSERT(header->sizeof_partition_entry <= sizeof(gpt_entry));

    entries_real_size = (u64)header->num_partition_entries * header->sizeof_partition_entry;
    entries_read_size = (u64)((header->num_partition_entries + 3) / 4) * 512;

    err = read_data(entries_buf, part_id, header->partition_entry_lba, entries_read_size);
    if (err) {
        efi_err("%sread entries(part_id=%d,lba=%llx), err(%d)\n",
                TAG, part_id, header->partition_entry_lba, err);
        return err;
    }

    calc_crc = efi_crc32((u8 *)entries, (u32)entries_real_size);

    if (header->partition_entry_array_crc32 != calc_crc) {
        efi_err("%scheck header, err(entries crc 0x%x!=0x%x(calc))\n",
                TAG, header->partition_entry_array_crc32, calc_crc);
        return 1;
    }

    for (i = 0; i < header->num_partition_entries; i++) {
        part_ptr[i].info = &g_dram_buf->meta_info[i];
        if ((entries[i].partition_name[0] & 0xFF00) == 0) {
            w2s(part_ptr[i].info->name, PART_META_INFO_NAMELEN, entries[i].partition_name, GPT_ENTRY_NAME_LEN);
        } else {
            memcpy(part_ptr[i].info->name, entries[i].partition_name, 64);
        }
        part_ptr[i].start_sect = (unsigned long)entries[i].starting_lba;
        part_ptr[i].nr_sects = (unsigned long)(entries[i].ending_lba - entries[i].starting_lba + 1);
        part_ptr[i].part_id = EMMC_PART_USER;
	part_ptr[i].part_attr = (unsigned long)entries[i].attributes;
        efi_info("%s[%d]name=%s, part_id=%d, start_sect=0x%x, nr_sects=0x%x\n", TAG, i, part_ptr[i].info->name,
                part_ptr[i].part_id, part_ptr[i].start_sect, part_ptr[i].nr_sects);
    }

    return 0;
}


int read_gpt(part_t *part)
{
    int err;
    u64 lba;
    u32 part_id = EMMC_PART_USER;

    part_ptr = part;

    efi_info("%sParsing Primary GPT now...\n", TAG);
    err = parse_gpt_header(part_id, 1, pgpt_header, pgpt_entries);
    if (!err) {
        goto find;
    }

    efi_info("%sParsing Secondary GPT now...\n", TAG);
    lba = last_lba(part_id);
    err = parse_gpt_header(part_id, lba, sgpt_header, sgpt_entries);
    if (!err) {
        goto find;
    }

    efi_err("%sFailure to find valid GPT.\n", TAG);
    return err;

find:
    efi_info("%sSuccess to find valid GPT.\n", TAG);
    return 0;
}

static int get_gpt_header(u32 part_id, u64 header_lba, u8 *header_buf, u32 len, u32 *crc32_tbl)
{
    int err;
    u32 calc_crc, orig_header_crc;
    gpt_header *header = (gpt_header *)header_buf;
    err = read_data(header_buf, part_id, header_lba, 512);//len);
    if (err) {
        efi_err("%sread header(part_id=%d,lba=%llx), err(%d)\n",
                TAG, part_id, header_lba, err);
        return err;
    }

    if (header->signature != GPT_HEADER_SIGNATURE) {
        efi_err("%scheck header, err(signature 0x%llx!=0x%llx)\n",
                TAG, header->signature, GPT_HEADER_SIGNATURE);
        return 1;
    }

    orig_header_crc = header->header_crc32;
    header->header_crc32 = 0;
    calc_crc = early_efi_crc32(~0L, (u8 *)header, header->header_size, crc32_tbl);
    calc_crc = early_efi_crc32_finalize(calc_crc);
    if (orig_header_crc != calc_crc) {
        efi_err("%scheck header, err(crc 0x%x!=0x%x(calc))\n",
                TAG, orig_header_crc, calc_crc);
        return 1;
    }

    header->header_crc32 = orig_header_crc;

    if (header->my_lba != header_lba) {
        efi_err("%scheck header, err(my_lba 0x%llx!=0x%llx)\n",
                TAG, header->my_lba, header_lba);
        return 1;
    }

    return 0;
}

#define PART_META_INFO_NAMELEN 64
static void get_part_name(u8 *dst, u16 *src, int len)
{
    int i = 0;
    if ((src[0] & 0xFF00) == 0) { // word to ascii string
        while (i < len - 1) {
            if (!src[i])
                break;
            dst[i] = src[i] & 0xFF;
            i++;
        }
        dst[i] = 0;
    } else {
        memcpy(dst, src, len - 1);
        dst[len-1] = 0;
    }
}

static void get_part_info(u32 part_id, part_t *part_ptr, gpt_entry *entry)
{
    get_part_name(part_ptr->info->name, entry->partition_name, min(PART_META_INFO_NAMELEN, GPT_ENTRY_NAME_LEN));
    part_ptr->start_sect = (unsigned long)entry->starting_lba;
    part_ptr->nr_sects = (unsigned long)(entry->ending_lba - entry->starting_lba + 1);
    part_ptr->part_id = part_id;
    part_ptr->part_attr = (unsigned long)entry->attributes;
}

static u64 get_part_ptr_by_gpt(u32 part_id, u64 header_lba, u8 *entries_buf, part_t *part_ptr,
		const char *part_name, u32 blksz) {
    int err, i, found;
    int entries_cnt, cur_entry_idx;
    int size_to_read;
    u32 calc_crc, orig_header_crc;
    u64 entries_real_size, entries_read_size, num_block_entry, read_lba;
    gpt_header *header = &gpt_header_buf;

    gpt_entry *entries = (gpt_entry *)entries_buf;
    early_init_crc32_table(gpt_sram_crc32_tbl);
    part_ptr->start_sect = 0;
    if (get_gpt_header(part_id, header_lba, (u8*) &gpt_header_buf, sizeof(gpt_header_buf), gpt_sram_crc32_tbl)) {
        efi_err("%s get_gpt_header fail\n", TAG);
        return 1;
    }

    num_block_entry = (u64)(blksz / header->sizeof_partition_entry);
    entries_read_size = (u64)((header->num_partition_entries + 3) / 4) * 512;

    calc_crc = ~0L;
    read_lba = header->partition_entry_lba;
    found = 0;
    while (entries_read_size > 0) {
        err = read_data((u8*)entries_buf, part_id, read_lba, GPT_SRAM_BUF_SIZE);
        read_lba = read_lba + GPT_SRAM_BUF_SIZE/blksz;
        calc_crc = early_efi_crc32(calc_crc, (u8 *)entries_buf, (u32)GPT_SRAM_BUF_SIZE, gpt_sram_crc32_tbl);
        entries_read_size -= GPT_SRAM_BUF_SIZE;
        entries_cnt = GPT_SRAM_BUF_SIZE/header->sizeof_partition_entry;

	for (i = 0; i < entries_cnt && found == 0; i++) {
            if(!entries[i].starting_lba){
                break;
	    }
            get_part_info(part_id, part_ptr, &entries[i]);

            if (!strcmp(part_ptr->info->name, part_name)) {
                found = 1;
                break;
            }
        }
    }

    calc_crc = early_efi_crc32_finalize(calc_crc);
    if (header->partition_entry_array_crc32 != calc_crc) {
        efi_err("%scheck header, err(entries crc 0x%x!=0x%x(calc)) found=0x%x \n",
            TAG, header->partition_entry_array_crc32, calc_crc, found);
        memset(part_ptr, 0, sizeof(part_t));
        return 1;
    }

    if (found == 0)
        memset(part_ptr, 0, sizeof(part_t));

    return 0;
}

int mt_get_part_info_by_name(const char *name, struct part_info_t *part_info)
{
    u32 part_id;

    part_id = EMMC_PART_USER;

    if (part_info == NULL || name == NULL) {
        efi_err("%s[%s]invalid argument\n", TAG, __func__);
        return 1;
    }
    blkdev_t *dev = blkdev_get(BOOTDEV_SDMMC);
    if (!dev) {
        efi_err("%sget_part_addr fail, err(no dev)\n", TAG);
        return 1;
    }
    gpt_sram_part_info.info = &gpt_sram_part_meta_info;
    if (get_part_ptr_by_gpt(part_id, 1, gpt_sram_buf, &gpt_sram_part_info, name, dev->blksz) != 0) {
        if (get_part_ptr_by_gpt(part_id, last_lba(part_id), gpt_sram_buf, &gpt_sram_part_info, name, dev->blksz) != 0) {
            efi_err("%sFailure to find valid GPT.\n", TAG);
            return 1;
        }
    }

    efi_info("%s startsec:%llx, partattr:%llx..\n", TAG, gpt_sram_part_info.start_sect, gpt_sram_part_info.part_attr);

    part_info->addr = (u64)gpt_sram_part_info.start_sect * (u64)dev->blksz;
    part_info->active = (gpt_sram_part_info.part_attr & PART_ATTR_LEGACY_BIOS_BOOTABLE);

    return 0;
}

u64 get_part_addr(const char *name) {
    struct part_info_t part_info;

    if (mt_get_part_info_by_name(name, &part_info) != 0) {
        efi_err("%smt_get_part_info_by_name fail\n", TAG);
        return 0;
    } else {
	return (u64)part_info.addr;
    }
}
