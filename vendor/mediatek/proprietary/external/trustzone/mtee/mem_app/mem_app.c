
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uree/system.h>
#include <uree/dbg.h>
#include <uree/mem.h>
#include <tz_cross/ta_dbg.h>
#include <tz_cross/ta_mem.h>
#include <unistd.h>

struct uree_memleak_dbg_s {
    uint64_t buffer;
    uint64_t LR;
    uint32_t size;
    uint32_t refcnt;
    int32_t tag_size;
    char tag[0];
};

#define MEM_LEAKAGE_INFO_SZ (1024*1024)

int main(int argc, char *argv[]) {
    TZ_RESULT ret;
    UREE_SESSION_HANDLE mem_session;
    UREE_SESSION_HANDLE dbg_session;
    UREE_SECUREMEM_INFO info;
    UREE_SHAREDMEM_PARAM shm_param;
    UREE_SHAREDMEM_HANDLE shm_handle = 0;
    MTEEC_PARAM param[4];
    int dump_leakage_info = 0;
    char *leakage_info;
    uint32_t leakage_info_sz = MEM_LEAKAGE_INFO_SZ;
    int opt;
    struct uree_memleak_dbg_s *membuf_item;
    char *prev_tag = NULL;
    int dump_with_tag = 0;
    char *tag_to_show = NULL;
    int dump_new = 0;
    int dump_flag;

    while ((opt = getopt(argc, argv, "lt:n")) != -1) {
        switch (opt) {
            case 'l':
               dump_leakage_info = 1;
               break;
            case 't':
               dump_with_tag = 1;
               tag_to_show = strdup(optarg);
               break;
            case 'n':
               dump_new = 1;
               break;
            default:
                fprintf(stderr, "Usage: %s [options]\n\
-l: dump leakage info (only available in eng build)\n\
-t <tag>: dump memory allocated with tag (only available in eng build)\n\
-n: dump newly allocated buffer since last time uree_mem is executed (only available in eng build)\n", argv[0]);
                return -1;
        }
    }

    printf("Run memory CA\n");

    ret = UREE_CreateSession(TZ_TA_DBG_UUID, &dbg_session);
    if (ret != TZ_RESULT_SUCCESS) {
        // Should provide strerror style error string in UREE.
        printf("CreateSession Error: %s\n", TZ_GetErrorString(ret));
        return 1;
    }

    // get secure memory information
    ret = UREE_GetSecurememinfo(dbg_session, &info);
    if (ret != TZ_RESULT_SUCCESS) {
        printf("UREE_GetSecurememinfo Error: %s\n", TZ_GetErrorString(ret));
        return 1;
    }
    printf("secure memory: Used = 0x%x bytes, \
Total = 0x%x bytes, Max free continuous mem sz = 0x%x bytes\n",
    info.used_byte, info.total_byte, info.max_free_cont_mem_sz);

    // get secure chunk memory information
    ret = UREE_GetSecurechunkmeminfo(dbg_session, &info);
    if (ret != TZ_RESULT_SUCCESS) {
        printf("UREE_GetSecurechunkmeminfo Error: %s\n", TZ_GetErrorString(ret));
        return 1;
    }
    printf("secure chunk memory: Used = 0x%x bytes, \
Total = 0x%x bytes, Max free continuous mem sz = 0x%x bytes\n",
    info.used_byte, info.total_byte, info.max_free_cont_mem_sz);

    /* try to get memleak information. */
    ret = UREE_CreateSession(TZ_TA_MEM_UUID, &mem_session);
    if (ret != TZ_RESULT_SUCCESS) {
        printf("CreateSession TZ_TA_MEM_UUID Error: %s\n", TZ_GetErrorString(ret));
        return -1;
    }

    if (dump_leakage_info != 0 || dump_with_tag != 0 || dump_new != 0) {
        leakage_info = (char *)calloc(leakage_info_sz, 1);
        if(leakage_info == NULL) {
            printf("malloc(%d) for leakage info fail!\n", leakage_info_sz);
            return -1;
        }

        shm_param.buffer = leakage_info;
        shm_param.size = leakage_info_sz;
        ret = UREE_RegisterSharedmemWithTag(mem_session, &shm_handle, &shm_param, "uree_mem");
        if (ret != TZ_RESULT_SUCCESS) {
            printf("UREE_RegisterSharedmem Error: %s\n", TZ_GetErrorString(ret));
            return -1;
        }

        param[0].memref.handle = shm_handle;
        param[0].memref.offset = 0;
        param[0].memref.size = leakage_info_sz;

        param[1].value.a = 0;
        ret = UREE_TeeServiceCall(dbg_session, TZCMD_DBG_SECUREMEM_MEMLEAK,
                TZ_ParamTypes2(TZPT_MEMREF_OUTPUT, TZPT_VALUE_OUTPUT), param);
        if (ret != TZ_RESULT_SUCCESS) {
            printf("TZCMD_DBG_SECUREMEM_MEMLEAK Error: %s\n", TZ_GetErrorString(ret));
        }

        printf("\nnew allocated memory buffer in secure memory:\n");
        membuf_item = (struct uree_memleak_dbg_s *)leakage_info;

        if (membuf_item->buffer == 0)
            printf("  (empty)\n");
        else {
            while(membuf_item->buffer != 0) {
                if(membuf_item->tag_size > 0)
                    prev_tag = membuf_item->tag;
                else if(membuf_item->tag_size == 0)
                    prev_tag = NULL;

                dump_flag = 1;
                if (dump_new == 1 && membuf_item->refcnt != 1)
                    dump_flag = 0;
                if (dump_with_tag == 1 && strncmp(tag_to_show, (prev_tag == NULL)?"":prev_tag, strlen(tag_to_show)) != 0)
                    dump_flag = 0;

                if(dump_flag != 0)
                    printf("buffer 0x%jx size (0x%08x) by PC:0x%jx tag:%s\n",
                        membuf_item->buffer, membuf_item->size, membuf_item->LR,
                        prev_tag);

                membuf_item = (struct uree_memleak_dbg_s *)
                              (((char *)membuf_item) +
                               sizeof(struct uree_memleak_dbg_s) + 1 + membuf_item->tag_size);
            }
        }


        memset(leakage_info, 0, leakage_info_sz);
        param[0].memref.handle = shm_handle;
        param[0].memref.offset = 0;
        param[0].memref.size = leakage_info_sz;

        param[1].value.a = 0;
        ret = UREE_TeeServiceCall(dbg_session, TZCMD_DBG_SECURECM_MEMLEAK,
                TZ_ParamTypes2(TZPT_MEMREF_OUTPUT, TZPT_VALUE_OUTPUT), param);
        if (ret != TZ_RESULT_SUCCESS) {
            printf("TZCMD_DBG_SECUREMEM_MEMLEAK Error: %s\n", TZ_GetErrorString(ret));
        }

        printf("\nnew allocated memory buffer in secure chunk:\n");
        membuf_item = (struct uree_memleak_dbg_s *)leakage_info;

        if (membuf_item->buffer == 0)
            printf("  (empty)\n");
        else {
            while(membuf_item->buffer != 0) {
                if(membuf_item->tag_size > 0)
                    prev_tag = membuf_item->tag;
                else if(membuf_item->tag_size == 0)
                    prev_tag = NULL;
                printf("buffer 0x%jx size (0x%08x) by PC:0x%jx tag:%s\n",
                    membuf_item->buffer, membuf_item->size, membuf_item->LR,
                    prev_tag);
                membuf_item = (struct uree_memleak_dbg_s *)
                              (((char *)membuf_item) +
                               sizeof(struct uree_memleak_dbg_s) + 1 + membuf_item->tag_size);
            }
        }

        ret = UREE_UnregisterSharedmem(mem_session, shm_handle);
        if (ret != TZ_RESULT_SUCCESS) {
            printf("UREE_UnregisterSharedmem Error: %s\n", TZ_GetErrorString(ret));
        }

        ret = UREE_CloseSession(mem_session);
        if (ret != TZ_RESULT_SUCCESS) {
            printf("CloseSeesion mem_session Error: %d\n", ret);
        }

        free(leakage_info);
        if (tag_to_show != NULL)
            free(tag_to_show);
    }
    ret = UREE_CloseSession(dbg_session);
    if (ret != TZ_RESULT_SUCCESS) {
        printf("CloseSeesion dbg_session_A Error: %d\n", ret);
    }

    printf("Memory CA done\n");

    return 0;
}
