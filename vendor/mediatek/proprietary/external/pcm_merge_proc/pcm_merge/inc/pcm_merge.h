#ifndef PCM_MERGE_H
#define PCM_MERGE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PCM_MERGE_MAX_CHANNEL 16

typedef void    merge_handle_t;

typedef int (*merge_read_callback)(void *pcm_buf, int size, long long tstamp, int flag, void *priv);

typedef enum {
	PCM_HW_ALSA_LIB = 0,
	PCM_HW_TINY_ALSA,
	PCM_HW_TYPE_NUM,
} pcm_hw_type_t;

enum {
	PCM_MERGE_FIRST_UNIT = 0x0001, /* first merge frame after start or re-merge*/
	PCM_MERGE_SUB_MUTE = 0x0002,   /* when main no drop mode, show sub data is mute */
	PCM_MERGE_SUB_PARTIAL_MUTE = 0x0004, /* when main no drop mode, show sub data is partial mute */
};

enum {
	PCM_MERGE_CONFIG_MAIN_NODROP = 0x0001, /* config pcm merge to no drop mode */
	PCM_MERGE_CONFIG_MASK = PCM_MERGE_CONFIG_MAIN_NODROP,
};

enum info_type {
	PCM_MERGE_INFO_VERSION,
	PCM_MERGE_INFO_REMAIN_SIZE, /* include merged data size, not include alsa and temp buffer */
};

struct merge_pcm_out_config {
	int chnum;
	int bitdepth;
	int rate;
	int period_count;
	int use_main_chnum;
	int use_sub_chnum;
	int chnum_idx[PCM_MERGE_MAX_CHANNEL];
};

struct merge_pcm_hw_config {
	pcm_hw_type_t type;
	char *name; /* string length max 32 byte */
	int chnum;
	int bitdepth;
	int rate;
	int period_count;
};

/* Config request */
/* 1. if use cb, merge_read will can not be used */
/* 2. main pcm object's sample rate should be same with out */
/* 3. if sub pcm object use src, it only support 2ch */
/* 4. sub's rate and main's rate must be multiple relation */
/* 5. main, sub not support 24bit record */
struct merge_config {
	struct merge_pcm_out_config out;
	struct merge_pcm_hw_config main;
	struct merge_pcm_hw_config sub;
	int period_time;         /* micro second */
	int offset_time;         /* offset = sub signal time - main signal time, nano second */
	merge_read_callback cb;
	void *cb_priv;
	int config_flags;        /* if not use, set to 0 */
};

/* create a pcm merge handle */
int merge_create(merge_handle_t **handle, struct merge_config *config);
/* create a pcm merge handle */
int merge_destroy(merge_handle_t *handle);
/* start pcm merge function */
int merge_start(merge_handle_t *handle);
/* stop pcm merge function */
int merge_stop(merge_handle_t *handle);
/* read merge pcm data, this function must be call after merge_start */
int merge_read(merge_handle_t *handle, void *pcm_buf, int *size, long long *tstamp, int *flag);
/* clear buffer data when merge is stoped */
int merge_reset(merge_handle_t *handle);
/* get merge information */
int merge_get_info(merge_handle_t *handle, int type);

int merge_alsa_id_to_string(int card, int device, char *name);
int merge_string_to_alsa_id(int *card, int *device, char *name);

#ifdef __cplusplus
}
#endif

#endif