#ifndef MSRC_H
#define MSRC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void    msrc_handle_t;

/* error value defination */
#define MSRC_OK         0
#define MSRC_EPERM      (-1)           /* not support */
#define MSRC_ENOMEM     (-12)          /* Out of memory */
#define MSRC_EINVAL     (-22)          /* Invalid argument */

typedef enum {
	MSRC_SIMPLE,
	MSRC_BLISRC,
	MSRC_CUSTOMER, /* just define for future, not used now */
} msrc_type_t;

typedef enum {
	MSRC_CREAT,
	MSRC_INIT,
	MSRC_UNINIT,
	MSRC_RUNNING,
} msrc_state_t;

typedef int (*msrc_callback_t)(void *cb_data, void **buffer, int req_frames);

typedef struct {
	msrc_type_t src_type;
	int chnum;
	int bitdepth;          //now only support 32bit sign-integer
	int samplerate_in;
	int samplerate_out;
	msrc_callback_t cb_obtain_in;  /*callback mode used to obtain input buffer*/
	msrc_callback_t cb_release_in;  /*callback mode used to release used input buffer*/
	void *cb_data; /*callback function user data*/
} msrc_config_t;

typedef struct
{
	void *in_buf;
	void *out_buf;
	int in_size;   /*input buffer size, frames*/
	int out_size;  /*output buffer size, frames*/
	int in_used;   /*input buffer consumed size, frames*/
	int out_used;  /*output buffer generated size, frames*/
} msrc_data_t;


int msrc_create(msrc_handle_t **handle, msrc_config_t config);
int msrc_destroy(msrc_handle_t *handle);
int msrc_reset(msrc_handle_t *handle);
int msrc_samplerate_reconfig(msrc_handle_t *handle, int samplerate_in, int samplerate_out);
int msrc_process(msrc_handle_t *handle, msrc_data_t *src_data);
int msrc_cb_read(msrc_handle_t *handle, void *buf, int frames);

#ifdef __cplusplus
}
#endif

#endif