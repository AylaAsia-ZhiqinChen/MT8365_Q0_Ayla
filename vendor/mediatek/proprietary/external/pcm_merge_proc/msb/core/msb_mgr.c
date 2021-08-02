#include "msb_mgr.h"

static inline int msb_wait(msb_mgr_t *mgr)
{
	int flag_wait;
	int flag_wake;
	int flag_interrupt;

	flag_wait = mgr->server ? CBLK_SERVER_WAIT : CBLK_CLIENT_WAIT;
	flag_wake = mgr->server ? CBLK_SERVER_WAKE : CBLK_CLIENT_WAKE;
	flag_interrupt = mgr->server ? CBLK_SERVER_INTERRUPT : CBLK_CLIENT_INTERRUPT;
	pthread_mutex_lock(&mgr->cblk->mutex);
	if (mgr->cblk->state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EPIPE;
	}
	if (mgr->cblk->state & flag_interrupt) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EINTR;
	}
	if (mgr->cblk->state & flag_wake) {
		mgr->cblk->state &= ~flag_wake;
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return 0;
	}
	mgr->cblk->state |= flag_wait;
	pthread_cond_wait(&mgr->cblk->cond, &mgr->cblk->mutex);
	pthread_mutex_unlock(&mgr->cblk->mutex);
	return 0;
}

static inline int msb_wake_up(msb_mgr_t *mgr)
{
	int flag_wait;
	int flag_wake;

	flag_wait = mgr->server ? CBLK_CLIENT_WAIT : CBLK_SERVER_WAIT;
	flag_wake = mgr->server ? CBLK_CLIENT_WAKE : CBLK_SERVER_WAKE;

	pthread_mutex_lock(&mgr->cblk->mutex);
	if (mgr->cblk->state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EPIPE;
	}
	if (mgr->cblk->state & flag_wait)
		mgr->cblk->state &= ~flag_wait;
	else
		mgr->cblk->state |= flag_wake;
	pthread_cond_signal(&mgr->cblk->cond);
	pthread_mutex_unlock(&mgr->cblk->mutex);
	return 0;
}

size_t msb_get_alloc_size(size_t size)
{
	size_t alloc_size;
	alloc_size = sizeof(msb_cblk_t) + size;
	return alloc_size;
}

int msb_create(msb_handle_t **handle, void *shm, size_t shm_size, int flag)
{
	msb_mgr_t *mgr = NULL;
	pthread_mutexattr_t mutexattr;
	pthread_condattr_t condattr;

	*handle = NULL;

	if (shm_size < sizeof(msb_cblk_t))
		return -EINVAL;

	mgr = (msb_mgr_t *)malloc(sizeof(msb_mgr_t));
	if (mgr == NULL)
		return -ENOMEM;

	mgr->cblk = (msb_cblk_t*)shm;

	if (flag & MSB_INIT_MASK) {
		mgr->cblk->buf_size = shm_size - sizeof(msb_cblk_t);
		mgr->cblk->wptr = 0;
		mgr->cblk->rptr = 0;
		mgr->cblk->data_size = 0;
		mgr->cblk->state = 0;
		if (flag & MSB_PROCESS_MASK)	{
			pthread_mutexattr_init(&mgr->cblk->mutexattr);
			pthread_mutexattr_setpshared(&mgr->cblk->mutexattr, PTHREAD_PROCESS_SHARED);
			pthread_condattr_init(&mgr->cblk->condattr);
			pthread_condattr_setpshared(&mgr->cblk->condattr, PTHREAD_PROCESS_SHARED);
			pthread_mutex_init(&mgr->cblk->mutex, &mgr->cblk->mutexattr);
			pthread_cond_init(&mgr->cblk->cond, &mgr->cblk->condattr);
		} else {
			pthread_mutex_init(&mgr->cblk->mutex, NULL);
			pthread_cond_init(&mgr->cblk->cond, NULL);
		}
	}
	mgr->base = (uint8_t*)shm + sizeof(msb_cblk_t);
	mgr->end = mgr->base + mgr->cblk->buf_size;
	mgr->server = flag & MSB_SERVER_MASK;
	mgr->init = flag & MSB_INIT_MASK;
	mgr->range = flag & MSB_PROCESS_MASK;

	*handle = (msb_handle_t *)mgr;

	return 0;
}

int msb_destroy(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	int flag_invalid;
	int self_wait;
	int remote_wait;

	if (mgr == NULL)
		return -EINVAL;
	flag_invalid = mgr->server ? CBLK_SERVER_INVALID : CBLK_CLIENT_INVALID;
	self_wait = mgr->server ? CBLK_SERVER_WAIT : CBLK_CLIENT_WAIT;
	remote_wait = mgr->server ? CBLK_CLIENT_WAIT : CBLK_SERVER_WAIT;

	pthread_mutex_lock(&mgr->cblk->mutex);
	mgr->cblk->state |= flag_invalid;
	if (mgr->cblk->state & remote_wait) {
		mgr->cblk->state &= ~remote_wait;
		pthread_cond_signal(&mgr->cblk->cond);
	}
	if (mgr->cblk->state & self_wait) {
		printf("Warning, msb destroy will force over msb obtain\n");
		mgr->cblk->state &= ~self_wait;
		pthread_cond_signal(&mgr->cblk->cond);
	}
	pthread_mutex_unlock(&mgr->cblk->mutex);
	if (mgr->init) {
		if (mgr->range) {
			pthread_mutexattr_destroy(&mgr->cblk->mutexattr);
			pthread_condattr_destroy(&mgr->cblk->condattr);
		}
		pthread_mutex_destroy(&mgr->cblk->mutex);
		pthread_cond_destroy(&mgr->cblk->cond);
	}
	free(mgr);
	return 0;
}

int msb_interrupt(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	int flag_interrupt;
	int flag_wait;

	if (mgr == NULL)
		return -EINVAL;
	flag_interrupt = mgr->server ? CBLK_SERVER_INTERRUPT : CBLK_CLIENT_INTERRUPT;
	flag_wait = mgr->server ? CBLK_SERVER_WAIT : CBLK_CLIENT_WAIT;

	pthread_mutex_lock(&mgr->cblk->mutex);
	if (mgr->cblk->state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EPIPE;
	}
	mgr->cblk->state |= flag_interrupt;
	if (mgr->cblk->state & flag_wait)
		pthread_cond_signal(&mgr->cblk->cond);
	pthread_mutex_unlock(&mgr->cblk->mutex);
	return 0;
}

int msb_obtain(msb_handle_t *handle, msb_buffer_t *buf, size_t req_size, int flag)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	msb_cblk_t *cblk;
	uint8_t *rptr;
	uint8_t *wptr;
	uint8_t *ptr;
	int full_obtain = 0;
	int block = 0;
	int wait = 0;
	int state, flag_interrupt;
	int ret = 0;
	size_t data_size, obtain_size, toend_size;

	if (mgr == NULL)
		return -EINVAL;
	if (buf == NULL)
		return -EINVAL;

	buf->buf = NULL;
	buf->buf_size = 0;
	buf->data_size = 0;
	if (req_size == 0)
		return 0;

	if (flag & MSB_OBTAIN_FULL)
		full_obtain = 1;
	if (flag & MSB_OBTAIN_BLOCK)
		block = 1;
	cblk = mgr->cblk;
	flag_interrupt = mgr->server ? CBLK_SERVER_INTERRUPT : CBLK_CLIENT_INTERRUPT;

	do {
		wait = 0;
		pthread_mutex_lock(&cblk->mutex);
		rptr = mgr->base + cblk->rptr;
		wptr = mgr->base + cblk->wptr;
		data_size = cblk->data_size;
		state = cblk->state;
		pthread_mutex_unlock(&cblk->mutex);
		if (state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
			ret = -EPIPE;
			break;
		}
		if (state & flag_interrupt) {
			pthread_mutex_lock(&cblk->mutex);
			cblk->state &= ~flag_interrupt;
			pthread_mutex_unlock(&cblk->mutex);
			ret = -EINTR;
			break;
		}
		if (mgr->server) {
			obtain_size = (data_size > req_size) ? req_size : data_size;
			toend_size = mgr->end - rptr;
			ptr = rptr;
		} else {
			obtain_size = ((cblk->buf_size - data_size) > req_size) ? req_size : (cblk->buf_size - data_size);
			toend_size = mgr->end - wptr;
			ptr = wptr;
		}

		if (obtain_size == 0 && block) {
			wait = 1;
		}
		if ((obtain_size < req_size) &&
				(obtain_size < toend_size) &&
				block &&
				full_obtain) {
			wait = 1;
		}
		if ((obtain_size < req_size) &&
				(obtain_size < toend_size) &&
				!block &&
				full_obtain) {
			obtain_size = 0;
		}

		if (wait) {
			msb_wait(mgr);
		} else {
			if (obtain_size > toend_size)
				obtain_size = toend_size;
			if (obtain_size != 0) {
				buf->u8buf = ptr;
				buf->buf_size = obtain_size;
				buf->data_size = obtain_size;
			}
		}

	} while(wait);
	return ret;
}

int msb_release(msb_handle_t *handle, const msb_buffer_t *buf)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	size_t ptr, base_ptr, old_ptr;

	if (mgr == NULL)
		return -EINVAL;
	if (buf == NULL || buf->buf == NULL)
		return -EINVAL;

	if (buf->buf_size == 0)
		return 0;
	ptr = buf->u8buf + buf->buf_size - mgr->base;
	base_ptr = buf->u8buf - mgr->base;
	if (ptr >= mgr->cblk->buf_size)
		ptr = ptr - mgr->cblk->buf_size;

	pthread_mutex_lock(&mgr->cblk->mutex);
	if (mgr->cblk->state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EPIPE;
	}
	if (mgr->server)
		old_ptr = mgr->cblk->rptr;
	else
		old_ptr = mgr->cblk->wptr;
	if (old_ptr != base_ptr) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		printf("%s Warning server:%d read point jump from:%d to %d\n",
																				__func__,
																				mgr->server,
																				(int)old_ptr,
																				(int)base_ptr);
		return -EINVAL;
	}
	if (mgr->server) {
		mgr->cblk->rptr = ptr;
		mgr->cblk->data_size -= buf->buf_size;
	} else {
		mgr->cblk->wptr = ptr;
		mgr->cblk->data_size += buf->buf_size;
	}
	pthread_mutex_unlock(&mgr->cblk->mutex);

	msb_wake_up(mgr);
	return 0;
}

size_t msb_get_remain_size(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	size_t size;

	if (mgr == NULL)
		return -EINVAL;

	pthread_mutex_lock(&mgr->cblk->mutex);
	if (mgr->cblk->state & (CBLK_SERVER_INVALID | CBLK_CLIENT_INVALID)) {
		pthread_mutex_unlock(&mgr->cblk->mutex);
		return -EPIPE;
	}
	if (mgr->server)
		size = mgr->cblk->data_size;
	else
		size = mgr->cblk->buf_size - mgr->cblk->data_size;
	pthread_mutex_unlock(&mgr->cblk->mutex);

	return size;
}

size_t msb_get_buffer_size(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	msb_cblk_t	*cblk;

	if (mgr == NULL)
		return -EINVAL;

	return mgr->cblk->buf_size;
}

void *msb_get_start_address(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	msb_cblk_t	*cblk;

	if (mgr == NULL)
		return NULL;

	return (void *)mgr->base;
}

int msb_reset_status(msb_handle_t *handle)
{
	msb_mgr_t *mgr = (msb_handle_t *)handle;
	msb_cblk_t	*cblk;
	int flag_interrupt;

	if (mgr == NULL)
		return -EINVAL;

	flag_interrupt = mgr->server ? CBLK_SERVER_INTERRUPT : CBLK_CLIENT_INTERRUPT;
	pthread_mutex_lock(&mgr->cblk->mutex);
	mgr->cblk->state &= ~flag_interrupt;
	pthread_mutex_unlock(&mgr->cblk->mutex);
	return 0;
}