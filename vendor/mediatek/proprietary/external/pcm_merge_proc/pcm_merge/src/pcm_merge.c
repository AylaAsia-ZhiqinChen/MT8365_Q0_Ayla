#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include "pcm_merge.h"
#include "pcm_hal.h"
#include "pcm_merge_mgr.h"

int merge_alsa_id_to_string(int card, int device, char *name)
{
	if (name == NULL)
		return -EINVAL;

	sprintf(name, "hw:%d,%d", card, device);

	return 0;
}

int merge_string_to_alsa_id(int *card, int *device, char *name)
{
	int ret;

	if (name == NULL)
		return -EINVAL;

	ret = sscanf(name, "hw:%d,%d", card, device);
	if (ret < 2)
		return -EINVAL;

	return 0;
}

static int merge_config_valid_check(struct merge_config *config)
{
	int i;

	/* output channel number check */
	if (config->out.chnum !=
		 (config->out.use_main_chnum + config->out.use_sub_chnum)) {
		print_err("out chnum(%d) not same with use_main_chnum(%d) + use_sub_chnum(%d)\n", config->out.chnum,
			config->out.use_main_chnum, config->out.use_sub_chnum);
		return -EINVAL;
	}

	if ((config->out.use_main_chnum > config->main.chnum) ||
		 (config->out.use_sub_chnum > config->sub.chnum)) {
		print_err("main chnum(%d) < use_main_ch(%d) or sub chnum(%d) < use_sub_ch(%d)",
			config->main.chnum,
			config->out.use_main_chnum,
			config->sub.chnum,
			config->out.use_sub_chnum);
		return -EINVAL;
	}

	/* out channel idx check */
	for (i = 0; i < config->out.chnum; i++) {
		if (i < config->out.use_main_chnum) {
			if (config->out.chnum_idx[i] < 0 || config->out.chnum_idx[i] >= config->main.chnum) {
				print_err("output channel(idx:%d, ch:%d) from main is not valid\n", i, config->out.chnum_idx[i]);
				return -EINVAL;
			}
		} else if (i >= config->out.use_main_chnum && i < config->out.chnum) {
			if (config->out.chnum_idx[i] < 0 || config->out.chnum_idx[i] >= config->sub.chnum) {
				print_err("output channel(idx:%d, ch:%d) from sub is not valid\n", i, config->out.chnum_idx[i]);
				return -EINVAL;
			}
		}
	}

	/* main pcm rate must same with out, only sub pcm support src*/
	if (config->out.rate != config->main.rate) {
		print_err("out rate(%d) not same with main rate(%d)\n", config->out.rate, config->main.rate);
		return -EINVAL;
	}

	if ((config->main.rate % config->sub.rate) != 0 &&
		(config->sub.rate % config->main.rate) != 0) {
		print_err("main rate(%d) sub rate(%d) are not same clock base\n", config->main.rate, config->sub.rate);
		return -EINVAL;
	}

	/* check bitdepth */
	if (config->main.bitdepth != 32 && config->main.bitdepth != 16) {
		print_err("main bitdepth(%d) is not valid\n", config->main.bitdepth);
		return -EINVAL;
	}

	if (config->sub.bitdepth != 32 && config->sub.bitdepth != 16) {
		print_err("main bitdepth(%d) is not valid\n", config->sub.bitdepth);
		return -EINVAL;
	}

	if (config->out.bitdepth != 32 && config->out.bitdepth != 16 && config->out.bitdepth != 24) {
		print_err("out bitdepth(%d) is not valid\n", config->sub.bitdepth);
		return -EINVAL;
	}

	/* check hardware name */
	if (config->main.name == NULL || config->sub.name == NULL) {
		print_err("main or sub name is null\n");
		return -EINVAL;
	}
	/* check period time */
	if (config->period_time == 0) {
		print_err("period time is 0\n");
		return -EINVAL;
	}

	/* check period count */
	if (config->main.period_count == 0 ||
		 config->sub.period_count == 0 ||
		 config->out.period_count == 0) {
		print_err("main/sub/out period count is 0\n");
		return -EINVAL;
	}

	if ((config->config_flags != 0) &&
		 ((config->config_flags & ~PCM_MERGE_CONFIG_MASK) != 0)) {
		print_err("invalid config_flags(0x%x)\n", config->config_flags);
		return -EINVAL;
	}
	return 0;
}

static int merge_config_parse(struct pcm_merge_mgr *mgr, struct merge_config *config)
{
	int period_size;
	int period_align;
	int merge_bits;

	period_size = config->period_time * config->out.rate / 1000;
	period_align = config->main.rate / config->sub.rate;
	if (period_align == 0)
		period_align = 16;
	else
		period_align *= 16;
	if (period_size % period_align != 0)
		period_size = (period_size / period_align) * period_align;

	merge_bits = (config->main.bitdepth >= config->sub.bitdepth) ? config->main.bitdepth : config->sub.bitdepth;

	strcpy(mgr->main_name, config->main.name);
	mgr->main_type = config->main.type;
	/* main audio in format */
	mgr->main_hal_fmt.rate = config->main.rate;
	mgr->main_hal_fmt.chnum = config->main.chnum;
	mgr->main_hal_fmt.bitdepth = config->main.bitdepth;
	mgr->main_hal_fmt.period_size = period_size;
	mgr->main_hal_fmt.period_count = config->main.period_count;
	/* main audio out format for merge */
	memcpy(&(mgr->main_out_fmt), &(mgr->main_hal_fmt), sizeof(struct pcm_format));
	mgr->main_out_fmt.bitdepth = merge_bits;

	print_info("===== main part=====\n");
	print_info("name:%s, type:%d\n", mgr->main_name, mgr->main_type);
	print_info("hal rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->main_hal_fmt.rate, mgr->main_hal_fmt.chnum, mgr->main_hal_fmt.bitdepth,
		        mgr->main_hal_fmt.period_size, mgr->main_hal_fmt.period_count);

	print_info("out rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->main_out_fmt.rate, mgr->main_out_fmt.chnum, mgr->main_out_fmt.bitdepth,
		        mgr->main_out_fmt.period_size, mgr->main_out_fmt.period_count);

	strcpy(mgr->sub_name, config->sub.name);
	mgr->sub_type = config->sub.type;
	/* sub audio in format */
	mgr->sub_hal_fmt.rate = config->sub.rate;
	mgr->sub_hal_fmt.chnum = config->sub.chnum;
	mgr->sub_hal_fmt.bitdepth = config->sub.bitdepth;
	mgr->sub_hal_fmt.period_size = period_size * config->sub.rate / config->main.rate;
	mgr->sub_hal_fmt.period_count = config->main.period_count;
	/* sub audio out format for merge */
	mgr->sub_out_fmt.rate = config->out.rate;
	mgr->sub_out_fmt.chnum = mgr->sub_hal_fmt.chnum;
	mgr->sub_out_fmt.bitdepth = merge_bits;
	mgr->sub_out_fmt.period_size = period_size;
	mgr->sub_out_fmt.period_count = config->sub.period_count;
	mgr->src_type = PCM_MERGE_SRC_TYPE;

	print_info("===== sub part=====\n");
	print_info("name:%s, type:%d\n", mgr->sub_name, mgr->sub_type);
	print_info("hal rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->sub_hal_fmt.rate, mgr->sub_hal_fmt.chnum, mgr->sub_hal_fmt.bitdepth,
		        mgr->sub_hal_fmt.period_size, mgr->sub_hal_fmt.period_count);

	print_info("out rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->sub_out_fmt.rate, mgr->sub_out_fmt.chnum, mgr->sub_out_fmt.bitdepth,
		        mgr->sub_out_fmt.period_size, mgr->sub_out_fmt.period_count);

	/* merge audio format */
	mgr->out_merge_fmt.rate = config->out.rate;
	mgr->out_merge_fmt.chnum = config->out.chnum;
	mgr->out_merge_fmt.bitdepth = merge_bits;
	mgr->out_merge_fmt.period_size = period_size;
	mgr->out_merge_fmt.period_count = config->out.period_count;
	mgr->use_main_chnum = config->out.use_main_chnum;
	mgr->use_sub_chnum = config->out.use_sub_chnum;
	for (int i = 0; i < config->out.chnum; i++)
		mgr->chnum_idx[i] = config->out.chnum_idx[i];

	/* main audio out format for merge */
	memcpy(&(mgr->out_fmt), &(mgr->out_merge_fmt), sizeof(struct pcm_format));
	mgr->out_fmt.bitdepth = config->out.bitdepth;

	mgr->ts_offset = (int64_t)(config->offset_time);
	mgr->cb = config->cb;
	mgr->cb_priv = config->cb_priv;

	print_info("===== out part=====\n");
	print_info("merge rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->out_merge_fmt.rate, mgr->out_merge_fmt.chnum, mgr->out_merge_fmt.bitdepth,
		        mgr->out_merge_fmt.period_size, mgr->out_merge_fmt.period_count);

	print_info("out rate(%d) chnum(%d) bitdepth(%d) period_size(%d) period_count(%d)\n",
		        mgr->out_fmt.rate, mgr->out_fmt.chnum, mgr->out_fmt.bitdepth,
		        mgr->out_fmt.period_size, mgr->out_fmt.period_count);
	print_info("use main chnum:%d, use sub chnum:%d\n", mgr->use_main_chnum, mgr->use_sub_chnum);
	print_info("out channel idx:");
	for (int i = 0; i < config->out.chnum; i++)
		print_info("%d, ", mgr->chnum_idx[i]);
	print_info("\n");
	print_info("ts offset:%lld\n", (long long)(mgr->ts_offset));
	if (mgr->cb)
		print_info("out has callback\n");
	else
		print_info("out has no callback\n");

	return 0;
}

static int merge_main_init(struct pcm_merge_mgr *mgr)
{
	int ret;
	struct pcm_hal_config hal_config;

	strcpy(hal_config.name, mgr->main_name);
	hal_config.rate = mgr->main_hal_fmt.rate;
	hal_config.chnum = mgr->main_hal_fmt.chnum;
	hal_config.bitdepth = mgr->main_hal_fmt.bitdepth;
	hal_config.period_size = mgr->main_hal_fmt.period_size;
	hal_config.buffer_size = mgr->main_hal_fmt.period_count * mgr->main_hal_fmt.period_size;
	hal_config.start_threshold = 0;

	ret = pcm_hal_create(&(mgr->main_hal), mgr->main_type);
	if (ret != 0)
		goto error_exit;

	ret = mgr->main_hal.pcm_open(&(mgr->main_hal), hal_config);
	if (ret != 0)
		goto error_exit_1;

	if (mgr->main_hal_fmt.bitdepth != mgr->main_out_fmt.bitdepth) {
		mgr->main_bitcvt.use = 1;
		mgr->main_bitcvt.bits_in = mgr->main_hal_fmt.bitdepth;
		mgr->main_bitcvt.bits_out = mgr->main_out_fmt.bitdepth;
		mgr->main_bitcvt.process = get_pcm_bits_convert_func(mgr->main_bitcvt.bits_in, mgr->main_bitcvt.bits_out);
		print_info("main int bits convert from %d to %d\n", mgr->main_bitcvt.bits_in, mgr->main_bitcvt.bits_out);
	} else {
		mgr->main_bitcvt.use = 0;
	}

	mgr->is_main_use = 1;
	mgr->is_main_task_enable = 0;

	return 0;

error_exit_1:
	pcm_hal_destroy(&(mgr->main_hal));
error_exit:
	return ret;
}

static int merge_main_uninit(struct pcm_merge_mgr *mgr)
{
	mgr->main_hal.pcm_close(&(mgr->main_hal));
	pcm_hal_destroy(&(mgr->main_hal));
	mgr->main_bitcvt.use = 0;
	mgr->is_main_use = 0;
	mgr->is_main_task_enable = 0;

	return 0;
}

static int merge_sub_init(struct pcm_merge_mgr *mgr)
{
	int ret;
	struct pcm_hal_config hal_config;

	strcpy(hal_config.name, mgr->sub_name);
	hal_config.rate = mgr->sub_hal_fmt.rate;
	hal_config.chnum = mgr->sub_hal_fmt.chnum;
	hal_config.bitdepth = mgr->sub_hal_fmt.bitdepth;
	hal_config.period_size = mgr->sub_hal_fmt.period_size;
	hal_config.buffer_size = mgr->sub_hal_fmt.period_count * mgr->sub_hal_fmt.period_size;
	hal_config.start_threshold = 0;

	ret = pcm_hal_create(&(mgr->sub_hal), mgr->sub_type);
	if (ret != 0)
		goto error_exit;

	ret = mgr->sub_hal.pcm_open(&(mgr->sub_hal), hal_config);
	if (ret != 0)
		goto error_exit_1;

	if (mgr->sub_hal_fmt.rate != mgr->sub_out_fmt.rate) {
		msrc_config_t config;
		if (mgr->src_type == 1)
			config.src_type = MSRC_BLISRC;
		else if (mgr->src_type == 2)
			config.src_type = MSRC_CUSTOMER; /* not use now */
		else
			config.src_type = MSRC_SIMPLE;

		config.chnum = mgr->sub_hal_fmt.chnum;
		config.bitdepth = mgr->sub_hal_fmt.bitdepth;
		config.samplerate_in = mgr->sub_hal_fmt.rate;
		config.samplerate_out = mgr->sub_out_fmt.rate;
		config.cb_obtain_in = NULL;
		config.cb_release_in = NULL;
		config.cb_data = NULL;
		ret = msrc_create(&(mgr->sub_src), config);
		if (ret != 0 || mgr->sub_src == NULL)
			goto error_exit_2;
		print_info("sub src convert from %d to %d with type(%d)\n", config.samplerate_in, config.samplerate_out, config.src_type);
	} else {
		mgr->sub_src = NULL;
	}

	ret = sharebuffer_tstamp_init(&(mgr->sub_out),
		                          mgr->sub_out_fmt.period_size,
		                          mgr->sub_out_fmt.period_count,
		                          (mgr->sub_out_fmt.bitdepth * mgr->sub_out_fmt.chnum / 8),
		                          mgr->sub_out_fmt.rate);
	if (ret != 0)
		goto error_exit_3;

	if (mgr->sub_hal_fmt.bitdepth != mgr->sub_out_fmt.bitdepth) {
		mgr->sub_bitcvt.use = 1;
		mgr->sub_bitcvt.bits_in = mgr->sub_hal_fmt.bitdepth;
		mgr->sub_bitcvt.bits_out = mgr->sub_out_fmt.bitdepth;
		mgr->sub_bitcvt.process = get_pcm_bits_convert_func(mgr->sub_bitcvt.bits_in, mgr->sub_bitcvt.bits_out);
		print_info("sub init bits convert from %d to %d\n", mgr->sub_bitcvt.bits_in, mgr->sub_bitcvt.bits_out);
	} else {
		mgr->sub_bitcvt.use = 0;
	}

	mgr->is_sub_use = 1;
	mgr->is_sub_task_enable = 0;

	return 0;

error_exit_3:
	if (mgr->sub_src != NULL)
		msb_destroy(mgr->sub_src);
error_exit_2:
	mgr->sub_hal.pcm_close(&(mgr->sub_hal));
error_exit_1:
	pcm_hal_destroy(&(mgr->sub_hal));
error_exit:
	return ret;
}

static int merge_sub_uninit(struct pcm_merge_mgr *mgr)
{
	sharebuffer_tstamp_uninit(&(mgr->sub_out));
	if (mgr->sub_src != NULL)
		msrc_destroy(mgr->sub_src);
	mgr->sub_hal.pcm_close(&(mgr->sub_hal));
	pcm_hal_destroy(&(mgr->sub_hal));
	mgr->sub_bitcvt.use = 0;
	mgr->is_sub_use = 0;
	mgr->is_sub_task_enable = 0;

	return 0;
}

static int merge_out_init(struct pcm_merge_mgr *mgr)
{
	int ret;

	ret = sharebuffer_tstamp_init(&(mgr->merge_out),
		                          mgr->out_merge_fmt.period_size,
		                          mgr->out_merge_fmt.period_count,
		                          (mgr->out_merge_fmt.bitdepth * mgr->out_merge_fmt.chnum / 8),
		                          mgr->out_merge_fmt.rate);
	if (ret != 0)
		return ret;

	if (mgr->out_merge_fmt.bitdepth != mgr->out_fmt.bitdepth) {
		mgr->out_bitcvt.use = 1;
		mgr->out_bitcvt.bits_in = mgr->out_merge_fmt.bitdepth;
		mgr->out_bitcvt.bits_out = mgr->out_fmt.bitdepth;
		mgr->out_bitcvt.process = get_pcm_bits_convert_func(mgr->out_bitcvt.bits_in, mgr->out_bitcvt.bits_out);
	} else {
		mgr->out_bitcvt.use = 0;
	}

	if (mgr->cb != NULL)
		mgr->is_out_use = 1;
	else
		mgr->is_out_use = 0;

	mgr->is_out_task_enable = 0;

	return 0;
}

static int merge_out_uninit(struct pcm_merge_mgr *mgr)
{
	sharebuffer_tstamp_uninit(&(mgr->merge_out));
	mgr->out_bitcvt.use = 0;
	mgr->is_out_use = 0;
	mgr->is_out_task_enable = 0;

	return 0;
}

static inline void merge_pcm_process(struct pcm_merge_mgr *mgr, void *main, void *sub, void *out, int frames)
{
	int out_chnum = mgr->out_merge_fmt.chnum;
	int main_chnum = mgr->main_out_fmt.chnum;
	int sub_chnum = mgr->sub_out_fmt.chnum;
	int bitdepth = mgr->out_merge_fmt.bitdepth;
	int use_main_chnum = mgr->use_main_chnum;
	int out_offset = 0;
	int main_offset = 0;
	int sub_offset = 0;
	int *ch_idx = mgr->chnum_idx;
	int i, j;

	if (bitdepth == 16) {
		for (i = 0; i < frames; i++) {
			for (j = 0; j < use_main_chnum; j++)
				*((int16_t *)out + out_offset + j) = *((int16_t *)main + main_offset + ch_idx[j]);

			for (j = use_main_chnum; j < out_chnum; j++)
				*((int16_t *)out + out_offset + j) = *((int16_t *)sub + sub_offset + ch_idx[j]);

			out_offset += out_chnum;
			main_offset += main_chnum;
			sub_offset += sub_chnum;
		}
	} else if (bitdepth == 32){
		for (i = 0; i < frames; i++) {
			for (j = 0; j < use_main_chnum; j++)
				*((int32_t *)out + out_offset + j) = *((int32_t *)main + main_offset + ch_idx[j]);

			for (j = use_main_chnum; j < out_chnum; j++)
				*((int32_t *)out + out_offset + j) = *((int32_t *)sub + sub_offset + ch_idx[j]);

			out_offset += out_chnum;
			main_offset += main_chnum;
			sub_offset += sub_chnum;
		}
	}
}

static inline int64_t merge_get_tolerance_time(struct pcm_merge_mgr *mgr)
{
	int main_frame, sub_frame;
	int64_t main_tolerance, sub_tolerance, tolerance;

	main_frame = PCM_POINT_HW_TOLERANCE_BITS / (mgr->main_hal_fmt.bitdepth * mgr->main_hal_fmt.chnum);
	sub_frame = PCM_POINT_HW_TOLERANCE_BITS / (mgr->sub_hal_fmt.bitdepth * mgr->sub_hal_fmt.chnum);

	main_tolerance = (int64_t)main_frame * TSTAMP_PER_SECOND / (int64_t)(mgr->main_hal_fmt.rate);
	sub_tolerance = (int64_t)sub_frame * TSTAMP_PER_SECOND / (int64_t)(mgr->sub_hal_fmt.rate);
	tolerance = main_tolerance >= sub_tolerance ? main_tolerance : sub_tolerance;
	tolerance = tolerance < SYNC_TOLERANCE_MIN ? SYNC_TOLERANCE_MIN : tolerance;

	if (tolerance % SYNC_TOERANCE_UNIT != 0)
		tolerance = (tolerance / SYNC_TOERANCE_UNIT + 1) * SYNC_TOERANCE_UNIT;

	return tolerance;
}

static void *main_process_task(void *arg)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)(arg);
	msb_buffer_t sub_unit;
	struct sharebuffer_tstamp *sub;
	struct sharebuffer_tstamp_mgr *sub_out = &(mgr->sub_out);
	msb_buffer_t merge_unit;
	struct sharebuffer_tstamp *merge;
	struct sharebuffer_tstamp_mgr *merge_out = &(mgr->merge_out);
	int64_t tstamp = 0;
	int64_t old_tstamp = 0;
	int64_t unit_time;
	int64_t main_dropping_time = 0;
	int64_t sub_dropping_time = 0;
	int read_size;
	uint8_t *read_buf = NULL;
	int bitcvt_buf_size;
	uint8_t *bitcvt_buf = NULL;
	uint8_t *out_buf;
	int state = MERGE_ST_PREPARE;
	int state_change = 0;
	int main_read_enable = 1;
	int sub_read_enable = 1;
	int64_t tolerance_time, offset_time;
	int ret;

	prctl(PR_SET_NAME, "merge_main");
	print_info("%s() thread task start\n", __FUNCTION__);
	unit_time = (int64_t)(mgr->main_hal_fmt.period_size) * TSTAMP_PER_SECOND / (int64_t)(mgr->main_hal_fmt.rate);
	tolerance_time = merge_get_tolerance_time(mgr);
	offset_time = mgr->ts_offset;

	print_info("%s() unit_time:%lld, tolerance_time:%lld\n", __FUNCTION__, (long long)unit_time, (long long)tolerance_time);

	read_size = mgr->main_hal_fmt.period_size;
	read_buf = (uint8_t *)malloc(read_size * mgr->main_hal_fmt.bitdepth * mgr->main_hal_fmt.chnum / 8);
	if (read_buf == NULL) {
		printf("%s(), main read buffer allocate fail!\n", __FUNCTION__);
		goto exit;
	}
	out_buf = read_buf;
	if (mgr->main_bitcvt.use) {
		bitcvt_buf_size = mgr->main_out_fmt.period_size * mgr->main_out_fmt.chnum * mgr->main_out_fmt.bitdepth / 8;
		bitcvt_buf = (uint8_t *)malloc(bitcvt_buf_size);
		if (bitcvt_buf == NULL) {
			print_err("%s(), main bitcvt buf allocate fail!\n", __FUNCTION__);
			goto exit;
		}
		out_buf = bitcvt_buf;
	}

	while(mgr->is_main_task_enable) {
		if (main_read_enable) {
			ret = mgr->main_hal.pcm_read(&(mgr->main_hal), (void *)read_buf, read_size, &tstamp);
			if (ret != 0) {
				print_err("%s(), main hal read fail(%d)\n", __FUNCTION__, ret);
				continue;
			}

			if (((tstamp - old_tstamp) > (unit_time + tolerance_time) ||
			    (tstamp - old_tstamp) < (unit_time - tolerance_time)) && old_tstamp != 0)
				print_warn("%s(), main time is jump from %lld to %lld (%lld:%lld)!!\n", __FUNCTION__, (long long)old_tstamp, (long long)tstamp, (long long)unit_time, (long long)tolerance_time);
			old_tstamp = tstamp;
		}
		if (sub_read_enable) {
			ret = msb_obtain(sub_out->server, &sub_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK | MSB_OBTAIN_FULL);
			if (ret < 0 || sub_unit.buf == NULL) {
				print_err("%s(), sub out data get fail(%d), stop main thread\n", __FUNCTION__, ret);
				break;
			}
			sub = (struct sharebuffer_tstamp *)sub_unit.buf;
		}

		if (state == MERGE_ST_PREPARE) {
			/* sub timestamp must in the range (main timestamp ~ main timestamp + unit_time) */
			if (tstamp <= (sub->tstamp - offset_time - unit_time)) {
				/* main is too old drop main data, read next packet */
				print_info("%s(), [DROP] main packet too old, drop it(%lld, %lld)!\n", __FUNCTION__, (long long)tstamp, (long long)(sub->tstamp));
				main_dropping_time += unit_time;
				main_read_enable = 1;
				sub_read_enable = 1;
				continue;
			} else if (tstamp > sub->tstamp - offset_time) {
				/* drop current sub data, read next; */
				msb_release(sub_out->server, &sub_unit);
				sub_dropping_time += unit_time;
				main_read_enable = 0;
				sub_read_enable = 1;
				print_info("%s(), [DROP] sub packet too old, drop it(%lld, %lld)!\n", __FUNCTION__, (long long)tstamp, (long long)(sub->tstamp));
				continue;
			} else if (tstamp > (sub->tstamp - offset_time - unit_time) && tstamp <= (sub->tstamp - offset_time)) {
				int move_bytes, offset;
				int left;
				int i;
				int frame_size;
				int64_t temp;

				frame_size = mgr->main_hal_fmt.bitdepth * mgr->main_hal_fmt.chnum / 8;
				main_dropping_time += ((sub->tstamp - offset_time) - tstamp);
				move_bytes = (int)((tstamp + unit_time - sub->tstamp + offset_time) * (int64_t)(mgr->main_hal_fmt.rate) / TSTAMP_PER_SECOND);
				left = (int)(((tstamp + unit_time - sub->tstamp + offset_time) * (int64_t)(mgr->main_hal_fmt.rate)) % TSTAMP_PER_SECOND);
				left = (left > 500000000) ? 1 : 0;
				move_bytes = (move_bytes + left) * frame_size;
				offset = read_size * frame_size - move_bytes;
				print_info("%s(), [TIMESTAMP_OK] timestamp check ok (%lld, %lld), move_bytes(%d), offset(%d)!\n", __FUNCTION__, (long long)tstamp, (long long)(sub->tstamp), move_bytes, offset);
				for (i = 0; i < move_bytes; i++)
					*(read_buf + i) = *(read_buf + offset + i);

				/* read left part from driver */
				ret = mgr->main_hal.pcm_read(&(mgr->main_hal), (void *)(read_buf + move_bytes), offset / frame_size, &temp);
				if (ret != 0) {
					print_info("%s(), main read left part error(%d), drop current packet!\n", __FUNCTION__, ret);
					continue;
				}
 				/* goto merge running state */
				print_info("%s(), [DROP] Dropping Main:%lld us, Sub:%lld us\n", __FUNCTION__, (long long)main_dropping_time, (long long)sub_dropping_time);
 				print_info("%s(), go to merge state!\n", __FUNCTION__);
				tstamp = sub->tstamp - offset_time;
				state = MERGE_ST_RUNNING;
				main_read_enable = 1;
				sub_read_enable = 1;
				old_tstamp = 0;
				state_change = 1;
				main_dropping_time = 0;
				sub_dropping_time = 0;
			}
		}

		if (state == MERGE_ST_RUNNING) {
			int remain_size;

			/* check the time stamp is ok or not with tolerance threshold */
			if ((tstamp - (sub->tstamp - offset_time)) >= tolerance_time || (sub->tstamp - offset_time - tstamp) >= tolerance_time) {
			  /* drop all the data */
				msb_release(sub_out->server, &sub_unit);
				state = MERGE_ST_PREPARE;
				print_err("%s(), [RESYNC] main&sub timestamp not correct (%lld, %lld), goto prepare state!\n", __FUNCTION__, (long long)tstamp, (long long)(sub->tstamp));
				continue;
			}
			/* main bits convert */
			if (mgr->main_bitcvt.use == 1)
				mgr->main_bitcvt.process(read_buf, bitcvt_buf, mgr->main_out_fmt.chnum, mgr->main_out_fmt.period_size);

			/* Obtain output buffer */
			remain_size = msb_get_remain_size(merge_out->client);
			if (remain_size == 0)
				print_warn("%s(), warning, merge output buffer is full\n", __FUNCTION__);
			ret = msb_obtain(merge_out->client, &merge_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK|MSB_OBTAIN_FULL);
			if (ret < 0 || merge_unit.buf == NULL || merge_unit.buf_size != sizeof(struct sharebuffer_tstamp)) {
				/* drop data */
				msb_release(sub_out->server, &sub_unit);
				print_err("%s merge output buffer get fail(%d)!\n", __FUNCTION__, ret);
				continue;
			}
			merge = (struct sharebuffer_tstamp *)merge_unit.buf;

			/* merge packet timestamp is equal to main timestamp */
			merge->tstamp = tstamp;
			if (state_change == 1) {
				merge->flag = PCM_MERGE_FIRST_UNIT;
				state_change = 0;
			}
			/* do merge action */
			merge_pcm_process(mgr, (void *)out_buf, (void *)(sub->buf), (void *)(merge->buf), merge_out->unit_frames);

			merge->data_size = merge_out->unit_size;
			msb_release(merge_out->client, &merge_unit);
			/* merge over, release sub packet */
			sub->data_size = 0;
			msb_release(sub_out->server, &sub_unit);
		}
	}
exit:
	if (read_buf != NULL)
		free(read_buf);
	if (bitcvt_buf != NULL)
		free(bitcvt_buf);
	print_info("%s() thread task over\n", __FUNCTION__);
	pthread_exit(NULL);
	return NULL;
}

static inline int tstamp_match_check(int64_t tstamp1, int64_t tstamp2, int64_t tolerance, int64_t *delta)
{
	*delta = tstamp2 - tstamp1;
	if ((tstamp2 - tstamp1 > tolerance) || (tstamp1 - tstamp2 > tolerance))
		return 0;
	else
		return 1;
}

static int sub_read_with_tstamp(struct pcm_merge_mgr *mgr, struct sub_read_request *request)
{
	int64_t sub_tstamp;
	int64_t sub_time_len;
	int sub_offset;
	int is_match = 0;
	int64_t delta;
	msb_buffer_t sub_unit;
	struct sharebuffer_tstamp *sub;
	struct sharebuffer_tstamp_mgr *sub_out = &(mgr->sub_out);
	int req_size;
	int64_t req_time_len;
	int64_t req_tstamp;
	uint8_t *out = request->buf;
	int total_mute_size = 0;
	int flag = 0;
	int ret = 0;

	req_size = request->size;
	req_time_len = (int64_t)(req_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);
	req_tstamp = request->tstamp;

	while (req_size > 0) {
		ret = msb_obtain(sub_out->server, &sub_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK | MSB_OBTAIN_FULL);
		if (ret < 0 || sub_unit.buf == NULL) {
			print_err("%s(), sub out data get fail(%d)\n", __FUNCTION__, ret);
			break;
		}
		sub = (struct sharebuffer_tstamp *)sub_unit.buf;
		sub_offset = sub_out->unit_size - sub->data_size;
		sub_time_len = (int64_t)(sub->data_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);
		sub_tstamp = sub->tstamp + (int64_t)(sub_offset / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);

		is_match = tstamp_match_check(req_tstamp, sub_tstamp, request->tolerance, &delta);
		if (is_match) {
			/* sub timestamp match the main timestamp         */
			/* |   sub data   |                               */
			/* |   main data  |                               */
			int read_size;

			read_size = (int)(sub->data_size) >= req_size ? req_size : (int)(sub->data_size);
			memcpy(out, sub->buf + sub_offset, read_size);

			out += read_size;
			req_size -= read_size;
			req_time_len = (int64_t)(req_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);
			req_tstamp += (int64_t)(read_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);

			sub->data_size -= read_size;
			if (sub->data_size == 0)
				msb_release(sub_out->server, &sub_unit);
		} else if (!is_match && (delta < 0 && -delta >= sub_time_len)) {
			/* sub data is too old, drop it, read next        */
			/* |   sub data   |                               */
			/*                  |   main data   |             */
			sub->data_size = 0;
			msb_release(sub_out->server, &sub_unit);
			print_info("%s(), [DROP] sub packet too old, drop it(%lld, %lld, size(ns):%lld, drop size(ns):%lld)!\n",
				__FUNCTION__,
				(long long)req_tstamp,
				(long long)sub_tstamp,
				(long long)sub_time_len,
				(long long)sub_time_len);
		} else if (!is_match && (delta < 0 && -delta < sub_time_len)) {
			/* sub data is a little old, drop some, read next */
			/* |   sub data   |                               */
			/*          |   main data   |                     */
			int read_size;
			int read_offset;
	
			read_offset =  (int)((-delta) * (int64_t)(sub_out->rate) / TSTAMP_PER_SECOND) * (int)(sub_out->frame_size);
			read_size = (int)(sub->data_size) - read_offset;
			read_size = (read_size >= req_size) ? req_size : read_size;

			memcpy(out, sub->buf + sub_offset + read_offset, read_size);
			print_info("%s(), [DROP] sub packet too old, drop it(%lld, %lld, size(ns):%lld, drop size(ns):%lld)!\n",
				__FUNCTION__,
				(long long)req_tstamp,
				(long long)sub_tstamp,
				(long long)sub_time_len,
				(long long)(-delta));

			out += read_size;
			req_size -= read_size;
			req_time_len = (int64_t)(req_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);
			req_tstamp += (int64_t)(read_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);

			sub->data_size -= (uint32_t)(read_offset + read_size);
			if (sub->data_size == 0)
				msb_release(sub_out->server, &sub_unit);
		} else if (!is_match && (delta > 0 && delta < req_time_len)) {
			/* main data is a little old, add some mute data  */
			/*      |   sub data   |                          */
			/* |   main data   |                              */
			int mute_size;
			int read_size;

			mute_size = (int)((delta) * (int64_t)(sub_out->rate) / TSTAMP_PER_SECOND) * (int)(sub_out->frame_size);
			memset(out, 0, mute_size);

			print_info("%s(), [MUTE] add sub mute(%lld, %lld, main size(byte):%d, mute size(byte):%d)!\n",
				__FUNCTION__,
				(long long)req_tstamp,
				(long long)sub_tstamp,
				req_size,
				mute_size);

			out += mute_size;
			req_size -= mute_size;
			total_mute_size += mute_size;

			read_size =  (int)(sub->data_size) >= req_size ? req_size : (int)(sub->data_size);
			memcpy(out, sub->buf + sub_offset, read_size);
			out += read_size;
			req_size -= read_size;
			req_time_len = (int64_t)(req_size / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);
			req_tstamp += (int64_t)((read_size + mute_size) / sub_out->frame_size) * TSTAMP_PER_SECOND / (int64_t)(sub_out->rate);

			sub->data_size -= read_size;
			if (sub->data_size == 0)
				msb_release(sub_out->server, &sub_unit);
		} else if(!is_match && (delta > 0 && delta >= req_time_len)) {
			/* main data is too old, add mute data            */
			/*                    |   sub data   |            */
			/* |   main data   |                              */
			int mute_size;

			mute_size = req_size;
			memset(out, 0, mute_size);
			print_info("%s(), [MUTE] add sub mute(%lld, %lld, main size(byte):%d, mute size(byte):%d)!\n",
				__FUNCTION__,
				(long long)req_tstamp,
				(long long)sub_tstamp,
				req_size,
				mute_size);
			out += mute_size;
			req_size -= mute_size;
			total_mute_size += mute_size;
		}
	}
	if (total_mute_size != 0 && total_mute_size == request->size)
		flag = PCM_MERGE_SUB_MUTE;
	else if (total_mute_size != 0 && total_mute_size < request->size)
		flag = PCM_MERGE_SUB_PARTIAL_MUTE;

	request->flag = flag;

	return ret;
}

static void *main_process_nodrop_task(void *arg)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)(arg);
	struct sharebuffer_tstamp_mgr *sub_out = &(mgr->sub_out);
	msb_buffer_t merge_unit;
	struct sharebuffer_tstamp *merge;
	struct sharebuffer_tstamp_mgr *merge_out = &(mgr->merge_out);
	int64_t tstamp = 0;
	int64_t old_tstamp = 0;
	int64_t unit_time;
	int read_size;
	uint8_t *read_buf = NULL;
	int bitcvt_buf_size;
	uint8_t *bitcvt_buf = NULL;
	uint8_t *out_buf;
	uint8_t *sub_buf = NULL;
	int sub_buf_size;
	int64_t tolerance_time, offset_time;
	struct sub_read_request sub_request;
	int ret;

	prctl(PR_SET_NAME, "merge_main");
	print_info("%s() thread task start\n", __FUNCTION__);
	unit_time = (int64_t)(mgr->main_hal_fmt.period_size) * TSTAMP_PER_SECOND / (int64_t)(mgr->main_hal_fmt.rate);
	tolerance_time = merge_get_tolerance_time(mgr);
	offset_time = mgr->ts_offset;

	print_info("%s() unit_time:%lld, tolerance_time:%lld\n", __FUNCTION__, (long long)unit_time, (long long)tolerance_time);

	read_size = mgr->main_hal_fmt.period_size;
	read_buf = (uint8_t *)malloc(read_size * mgr->main_hal_fmt.bitdepth * mgr->main_hal_fmt.chnum / 8);
	if (read_buf == NULL) {
		printf("%s(), main read buffer allocate fail!\n", __FUNCTION__);
		goto exit;
	}
	out_buf = read_buf;
	if (mgr->main_bitcvt.use) {
		bitcvt_buf_size = mgr->main_out_fmt.period_size * mgr->main_out_fmt.chnum * mgr->main_out_fmt.bitdepth / 8;
		bitcvt_buf = (uint8_t *)malloc(bitcvt_buf_size);
		if (bitcvt_buf == NULL) {
			print_err("%s(), main bitcvt buf allocate fail!\n", __FUNCTION__);
			goto exit;
		}
		out_buf = bitcvt_buf;
	}

	sub_buf_size = mgr->main_hal_fmt.period_size * sub_out->frame_size;
	sub_buf = (uint8_t *)malloc(sub_buf_size);
	if (sub_buf == NULL) {
		print_err("%s(), sub buf allocate fail!\n", __FUNCTION__);
		goto exit;
	}

	sub_request.buf = sub_buf;
	sub_request.size = sub_buf_size;

	while(mgr->is_main_task_enable) {
		int remain_size;

		ret = mgr->main_hal.pcm_read(&(mgr->main_hal), (void *)read_buf, read_size, &tstamp);
		if (ret != 0) {
			print_err("%s(), main hal read fail(%d)\n", __FUNCTION__, ret);
			continue;
		}

		if (((tstamp - old_tstamp) > (unit_time + tolerance_time) ||
		    (tstamp - old_tstamp) < (unit_time - tolerance_time)) && old_tstamp != 0)
			print_warn("%s(), main time is jump from %lld to %lld (%lld:%lld)!!\n", __FUNCTION__, (long long)old_tstamp, (long long)tstamp, (long long)unit_time, (long long)tolerance_time);
		old_tstamp = tstamp;

		sub_request.tstamp = tstamp + offset_time;
		sub_request.tolerance = tolerance_time;
		ret = sub_read_with_tstamp(mgr, &sub_request);
		if (ret != 0) {
			print_err("%s(), sub out data get fail(%d), stop main thread\n", __FUNCTION__, ret);
			break;
		}

		/* main bits convert */
		if (mgr->main_bitcvt.use == 1)
			mgr->main_bitcvt.process(read_buf, bitcvt_buf, mgr->main_out_fmt.chnum, mgr->main_out_fmt.period_size);

		/* Obtain output buffer */
		remain_size = msb_get_remain_size(merge_out->client);
		if (remain_size == 0)
			print_warn("%s(), warning, merge output buffer is full\n", __FUNCTION__);
		ret = msb_obtain(merge_out->client, &merge_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK|MSB_OBTAIN_FULL);
		if (ret < 0 || merge_unit.buf == NULL || merge_unit.buf_size != sizeof(struct sharebuffer_tstamp)) {
			print_err("%s merge output buffer get fail(%d)!\n", __FUNCTION__, ret);
			continue;
		}
		merge = (struct sharebuffer_tstamp *)merge_unit.buf;

		/* merge packet timestamp is equal to main timestamp */
		merge->tstamp = tstamp;
		merge->flag = sub_request.flag;

		/* do merge action */
		merge_pcm_process(mgr, (void *)out_buf, (void *)(sub_buf), (void *)(merge->buf), merge_out->unit_frames);

		merge->data_size = merge_out->unit_size;
		msb_release(merge_out->client, &merge_unit);
	}
exit:
	if (read_buf != NULL)
		free(read_buf);
	if (bitcvt_buf != NULL)
		free(bitcvt_buf);
	if (sub_buf != NULL)
		free(sub_buf);
	print_info("%s() thread task over\n", __FUNCTION__);
	pthread_exit(NULL);
	return NULL;
}

static void *sub_process_task(void *arg)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)(arg);
	msb_buffer_t sub_unit;
	struct sharebuffer_tstamp *sub;
	struct sharebuffer_tstamp_mgr *sub_out = &(mgr->sub_out);
	int64_t tstamp = 0;
	int64_t old_tstamp = 0;
	int64_t unit_time, tolerance_time;
	int read_size;
	int src_size;
	uint8_t *read_buf = NULL;
	uint8_t *src_buf = NULL;
	uint8_t *out_buf = NULL;
	size_t remain_size;
	int firstSrc = 1;
	int ret;

	prctl(PR_SET_NAME, "merge_sub");
	print_info("%s() thread task start\n", __FUNCTION__);
	tolerance_time = merge_get_tolerance_time(mgr);
	unit_time = (int64_t)(mgr->sub_hal_fmt.period_size) * TSTAMP_PER_SECOND / (int64_t)(mgr->sub_hal_fmt.rate);

	print_info("%s() unit_time:%lld, tolerance_time:%lld\n", __FUNCTION__, (long long)unit_time, (long long)tolerance_time);

	read_size = mgr->sub_hal_fmt.period_size;
	read_buf = (uint8_t *)malloc(read_size * mgr->sub_hal_fmt.bitdepth * mgr->sub_hal_fmt.chnum / 8);
	if (read_buf == NULL) {
		print_err("%s(), sub read buffer allocate fail!\n", __FUNCTION__);
		goto exit;
	}
	out_buf = read_buf;

	if (mgr->sub_src != NULL) {
		src_size = mgr->sub_out_fmt.period_size;
		src_buf = (uint8_t *)malloc(src_size * mgr->sub_out_fmt.bitdepth * mgr->sub_out_fmt.chnum / 8);
		if (src_buf == NULL) {
			print_err("%s(), sub src buf allocate fail!\n", __FUNCTION__);
			goto exit;
		}
		out_buf = src_buf;
	}

	while(mgr->is_sub_task_enable) {
		ret = mgr->sub_hal.pcm_read(&(mgr->sub_hal), (void *)read_buf, read_size, &tstamp);
		if (ret != 0) {
			print_err("%s(), sub hal read fail(%d)\n", __FUNCTION__, ret);
			continue;
		}
		if (((tstamp - old_tstamp) > (unit_time + tolerance_time) ||
		    (tstamp - old_tstamp) < (unit_time - tolerance_time)) && old_tstamp != 0)
			print_err("%s(), sub time is jump from %lld to %lld!! (%lld:%lld)\n", __FUNCTION__, (long long)old_tstamp, (long long)tstamp, (long long)unit_time, (long long)tolerance_time);
		old_tstamp = tstamp;

		if (mgr->sub_src != NULL) {
			msrc_data_t data;

			data.in_buf = read_buf;
			data.in_size = read_size;
			data.in_used = 0;
			data.out_buf = src_buf;
			data.out_size = src_size;
			data.out_used = 0;
			msrc_process(mgr->sub_src, &data);
			if (data.in_used != read_size || data.out_used != src_size) {
				print_warn("%s(), sub 48=>16 src, input size(%d,%d) output size (%d,%d), first src(%d)\n", __FUNCTION__, data.in_used, data.in_size, data.out_used, data.out_size, firstSrc);
				/* drop and read next data; */
				if (firstSrc == 1)
					firstSrc = 0;
				continue;
			}
		}
		remain_size = msb_get_remain_size(sub_out->client);
		if (remain_size == 0)
			print_warn("%s(), warning, sub output buffer is full\n", __FUNCTION__);
		ret = msb_obtain(sub_out->client, &sub_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK|MSB_OBTAIN_FULL);
		if (ret < 0 || sub_unit.buf == NULL || sub_unit.buf_size != sizeof(struct sharebuffer_tstamp)) {
			print_err("%s sub output buffer get fail(%d)!\n", __FUNCTION__, ret);
			continue;
		}
		sub = (struct sharebuffer_tstamp *)sub_unit.buf;

		if (mgr->sub_bitcvt.use == 1) {
			mgr->sub_bitcvt.process(out_buf, sub->buf, mgr->sub_out_fmt.chnum, sub_out->unit_frames);
		} else {
			int copy_size = sub_out->unit_frames * mgr->sub_out_fmt.chnum * mgr->sub_out_fmt.bitdepth / 8;
			memcpy(sub->buf, out_buf, copy_size);
		}

		sub->data_size = sub->buf_size;
		sub->tstamp = tstamp;
		msb_release(sub_out->client, &sub_unit);
	}

exit:
	if (read_buf != NULL)
		free(read_buf);
	if (src_buf != NULL)
		free(src_buf);
	print_info("%s() thread task over\n", __FUNCTION__);
	pthread_exit(NULL);
	return NULL;
}

static void *out_process_task(void *arg)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)(arg);
	msb_buffer_t merge_unit;
	struct sharebuffer_tstamp *merge;
	struct sharebuffer_tstamp_mgr *merge_out = &(mgr->merge_out);
	int bitcvt_buf_size;
	uint8_t *bitcvt_buf = NULL;
	uint8_t *out_buf;
	int ret;

	prctl(PR_SET_NAME, "merge_out");
	print_info("%s() thread task start\n", __FUNCTION__);

	if (mgr->out_bitcvt.use) {
		bitcvt_buf_size = mgr->out_fmt.period_size * mgr->out_fmt.chnum * mgr->out_fmt.bitdepth / 8;
		bitcvt_buf = (uint8_t *)malloc(bitcvt_buf_size);
		if (bitcvt_buf == NULL)
			goto exit;
	}

	while(mgr->is_out_task_enable) {
		ret = msb_obtain(merge_out->server, &merge_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK|MSB_OBTAIN_FULL);
		if (ret < 0 || merge_unit.buf == NULL || merge_unit.buf_size != sizeof(struct sharebuffer_tstamp)) {
			print_err("%s(), read merge out fail(%d)!\n", __FUNCTION__, ret);
			continue;
		}
		merge = (struct sharebuffer_tstamp *)(merge_unit.buf);
		if (mgr->out_bitcvt.use == 1) {
			mgr->out_bitcvt.process(merge->buf, bitcvt_buf, mgr->out_fmt.chnum, merge_out->unit_frames);
			out_buf = bitcvt_buf;
		} else {
			out_buf = (uint8_t *)(merge->buf);
		}
		mgr->cb((void *)out_buf, merge_out->unit_frames, (long long)(merge->tstamp), (int)(merge->flag), mgr->cb_priv);

		merge->data_size = 0;
		merge->tstamp = 0;
		merge->flag = 0;
		msb_release(merge_out->server, &merge_unit);
	}

	if (bitcvt_buf != NULL)
		free(bitcvt_buf);
exit:
	print_info("%s() thread task over\n", __FUNCTION__);
	pthread_exit(NULL);
	return NULL;
}

int merge_create(merge_handle_t **handle, struct merge_config *config)
{
	struct pcm_merge_mgr *mgr;
	int ret;

	*handle = NULL;

	ret = merge_config_valid_check(config);
	if (ret < 0)
		return ret;

	mgr = (struct pcm_merge_mgr *)calloc(1, sizeof(struct pcm_merge_mgr));
	if (mgr == NULL)
		return -ENOMEM;

	merge_config_parse(mgr, config);

	ret = merge_main_init(mgr);
	if (ret != 0)
		goto error_exit;

	ret = merge_sub_init(mgr);
	if (ret != 0)
		goto error_exit1;

	ret = merge_out_init(mgr);
	if (ret != 0)
		goto error_exit2;

	mgr->state = PCM_MERGE_ST_STOP;
	mgr->config = config->config_flags;
	print_info("pcm merge config:0x%x\n", mgr->config);
	*handle = (merge_handle_t *)mgr;
	print_info("pcm merge create success!\n");
	return 0;
error_exit2:
	merge_sub_uninit(mgr);
error_exit1:
	merge_main_uninit(mgr);
error_exit:
	free(mgr);
	print_err("pcm merge create fail(%d)", ret);
	return ret;
}

int merge_destroy(merge_handle_t *handle)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;

	if (mgr == NULL)
		return -EINVAL;

	if (mgr->state == PCM_MERGE_ST_START)
		merge_stop(handle);
	merge_main_uninit(mgr);
	merge_sub_uninit(mgr);
	merge_out_uninit(mgr);
	free(mgr);
	print_info("pcm merge destroy success!\n");
	return 0;
}

int merge_read(merge_handle_t *handle, void *pcm_buf, int *size, long long *tstamp, int *flag)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;
	msb_buffer_t merge_unit;
	struct sharebuffer_tstamp *merge;
	struct sharebuffer_tstamp_mgr *merge_out = &(mgr->merge_out);
	int ret;

	if (mgr == NULL)
		return -EINVAL;

	if (mgr->state != PCM_MERGE_ST_START)
		return -EPERM;

	if (*size < mgr->out_fmt.period_size)
		return -EINVAL;

	ret = msb_obtain(merge_out->server, &merge_unit, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_BLOCK|MSB_OBTAIN_FULL);
	if (ret < 0 || merge_unit.buf == NULL || merge_unit.buf_size != sizeof(struct sharebuffer_tstamp)) {
		printf("%s(), read merge out fail(%d)!\n", __FUNCTION__, ret);
		return ret;
	}

	merge = (struct sharebuffer_tstamp *)(merge_unit.buf);
	if (mgr->out_bitcvt.use == 1) {
		mgr->out_bitcvt.process(merge->buf, pcm_buf, mgr->out_fmt.chnum, merge_out->unit_frames);
	} else {
		int copy_size = merge_out->unit_frames * mgr->out_fmt.chnum * mgr->out_fmt.bitdepth / 8;
		memcpy(pcm_buf, merge->buf, copy_size);
	}
	*size = merge_out->unit_frames;
	if (tstamp != NULL)
		*tstamp = (long long)merge->tstamp;

	if (flag != NULL)
		*flag = (int)(merge->flag);

	merge->data_size = 0;
	merge->tstamp = 0;
	merge->flag = 0;
	msb_release(merge_out->server, &merge_unit);

	return 0;
}

int merge_start(merge_handle_t *handle)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;
	int ret;

	if (mgr == NULL)
		return -EINVAL;

	if (mgr->state == PCM_MERGE_ST_START)
		return -EPERM;

	if (mgr->is_main_use) {
		mgr->is_main_task_enable = 1;
		ret = mgr->main_hal.pcm_start(&(mgr->main_hal));
		if (ret != 0)
			goto err_exit;

		if ((mgr->config & PCM_MERGE_CONFIG_MAIN_NODROP) != 0)
			ret = pthread_create(&(mgr->h_main_thread), NULL, main_process_nodrop_task, (void *)mgr);
		else
			ret = pthread_create(&(mgr->h_main_thread), NULL, main_process_task, (void *)mgr);
		if (ret != 0)
			goto err_exit;
	}

	if (mgr->is_sub_use) {
		mgr->is_sub_task_enable = 1;
		ret = mgr->sub_hal.pcm_start(&(mgr->sub_hal));
		if (ret != 0)
			goto err_exit;

		ret = pthread_create(&(mgr->h_sub_thread), NULL, sub_process_task, (void *)mgr);
		if (ret != 0)
			goto err_exit;
	}

	if (mgr->is_out_use) {
		mgr->is_out_task_enable = 1;

		ret = pthread_create(&(mgr->h_out_thread), NULL, out_process_task, (void *)mgr);
		if (ret != 0)
			goto err_exit;
	}
	print_info("pcm merge start success!\n");
	mgr->state = PCM_MERGE_ST_START;
	return 0;

err_exit:
	print_err("pcm merge start fail(%d)", ret);
	return ret;
}

int merge_stop(merge_handle_t *handle)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;

	if (mgr == NULL)
		return -EINVAL;

	if (mgr->state == PCM_MERGE_ST_STOP)
		return -EPERM;

	if (mgr->is_main_use)
		mgr->is_main_task_enable = 0;
	if (mgr->is_sub_use)
		mgr->is_sub_task_enable = 0;
	if (mgr->is_out_use)
		mgr->is_out_task_enable = 0;
	if (mgr->is_sub_use)
		sharebuffer_tstamp_interrupt(&(mgr->sub_out));
	sharebuffer_tstamp_interrupt(&(mgr->merge_out));

	if (mgr->is_main_use)
		pthread_join(mgr->h_main_thread, NULL);
	if (mgr->is_sub_use)
		pthread_join(mgr->h_sub_thread, NULL);
	if (mgr->is_out_use)
		pthread_join(mgr->h_out_thread, NULL);

	if (mgr->is_main_use)
		mgr->main_hal.pcm_stop(&(mgr->main_hal));
	if (mgr->is_sub_use)
		mgr->sub_hal.pcm_stop(&(mgr->sub_hal));

	print_info("pcm merge stop success!\n");
	mgr->state = PCM_MERGE_ST_STOP;
	return 0;
}

int merge_reset(merge_handle_t *handle)
{
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;

	if (mgr == NULL)
		return -EINVAL;

	if (mgr->state != PCM_MERGE_ST_STOP)
		return -EPERM;

	/* clear all the share buffer */
	if (mgr->is_sub_use)
		sharebuffer_tstamp_reset(&(mgr->sub_out));
	sharebuffer_tstamp_reset(&(mgr->merge_out));
	print_info("pcm merge reset success!\n");
	return 0;
}

int merge_get_info(merge_handle_t *handle, int type)
{
	int ret = 0;
	struct pcm_merge_mgr *mgr = (struct pcm_merge_mgr *)handle;

	switch (type) {
	case PCM_MERGE_INFO_VERSION:
		ret = (int)(PCM_MERGE_VERSION);
		break;
	case PCM_MERGE_INFO_REMAIN_SIZE:
		ret = sharebuffer_tstamp_get_remain_size(&(mgr->merge_out));
		break;
	}

	return ret;
}
