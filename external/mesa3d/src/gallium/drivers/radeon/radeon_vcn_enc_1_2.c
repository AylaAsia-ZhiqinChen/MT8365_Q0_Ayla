/**************************************************************************
 *
 * Copyright 2017 Advanced Micro Devices, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include <stdio.h>

#include "pipe/p_video_codec.h"

#include "util/u_video.h"
#include "util/u_memory.h"

#include "vl/vl_video_buffer.h"

#include "r600_pipe_common.h"
#include "radeon_video.h"
#include "radeon_vcn_enc.h"

#define RADEON_ENC_CS(value) (enc->cs->current.buf[enc->cs->current.cdw++] = (value))
#define RADEON_ENC_BEGIN(cmd) { \
	uint32_t *begin = &enc->cs->current.buf[enc->cs->current.cdw++]; \
RADEON_ENC_CS(cmd)
#define RADEON_ENC_READ(buf, domain, off) radeon_enc_add_buffer(enc, (buf), RADEON_USAGE_READ, (domain), (off))
#define RADEON_ENC_WRITE(buf, domain, off) radeon_enc_add_buffer(enc, (buf), RADEON_USAGE_WRITE, (domain), (off))
#define RADEON_ENC_READWRITE(buf, domain, off) radeon_enc_add_buffer(enc, (buf), RADEON_USAGE_READWRITE, (domain), (off))
#define RADEON_ENC_END() *begin = (&enc->cs->current.buf[enc->cs->current.cdw] - begin) * 4; \
	enc->total_task_size += *begin;}

static const unsigned profiles[7] = { 66, 77, 88, 100, 110, 122, 244 };
static const unsigned index_to_shifts[4] = {24, 16, 8, 0};

static void radeon_enc_add_buffer(struct radeon_encoder *enc, struct pb_buffer *buf,
								  enum radeon_bo_usage usage, enum radeon_bo_domain domain,
								  signed offset)
{
	enc->ws->cs_add_buffer(enc->cs, buf, usage | RADEON_USAGE_SYNCHRONIZED,
									   domain, RADEON_PRIO_VCE);
	uint64_t addr;
	addr = enc->ws->buffer_get_virtual_address(buf);
	addr = addr + offset;
	RADEON_ENC_CS(addr >> 32);
	RADEON_ENC_CS(addr);
}

static void radeon_enc_set_emulation_prevention(struct radeon_encoder *enc, bool set)
{
	if (set != enc->emulation_prevention) {
		enc->emulation_prevention = set;
		enc->num_zeros = 0;
	}
}

static void radeon_enc_output_one_byte(struct radeon_encoder *enc, unsigned char byte)
{
	if (enc->byte_index == 0)
		enc->cs->current.buf[enc->cs->current.cdw] = 0;
	enc->cs->current.buf[enc->cs->current.cdw] |= ((unsigned int)(byte) << index_to_shifts[enc->byte_index]);
	enc->byte_index++;

	if (enc->byte_index >= 4) {
		enc->byte_index = 0;
		enc->cs->current.cdw++;
	}
}

static void radeon_enc_emulation_prevention(struct radeon_encoder *enc, unsigned char byte)
{
	if(enc->emulation_prevention) {
		if((enc->num_zeros >= 2) && ((byte == 0x00) || (byte == 0x01) || (byte == 0x03))) {
            radeon_enc_output_one_byte(enc, 0x03);
            enc->bits_output += 8;
            enc->num_zeros = 0;
        }
        enc->num_zeros = (byte == 0 ? (enc->num_zeros + 1) : 0);
    }
}

static void radeon_enc_code_fixed_bits(struct radeon_encoder *enc, unsigned int value, unsigned int num_bits)
{
	unsigned int bits_to_pack = 0;

	while(num_bits > 0) {
		unsigned int value_to_pack = value & (0xffffffff >> (32 - num_bits));
		bits_to_pack = num_bits > (32 - enc->bits_in_shifter) ? (32 - enc->bits_in_shifter) : num_bits;

		if (bits_to_pack < num_bits)
			value_to_pack = value_to_pack >> (num_bits - bits_to_pack);

		enc->shifter |= value_to_pack << (32 - enc->bits_in_shifter - bits_to_pack);
		num_bits -= bits_to_pack;
		enc->bits_in_shifter += bits_to_pack;

		while(enc->bits_in_shifter >= 8) {
			unsigned char output_byte = (unsigned char)(enc->shifter >> 24);
			enc->shifter <<= 8;
			radeon_enc_emulation_prevention(enc, output_byte);
			radeon_enc_output_one_byte(enc, output_byte);
			enc->bits_in_shifter -= 8;
			enc->bits_output += 8;
		}
	}
}

static void radeon_enc_reset(struct radeon_encoder *enc)
{
	enc->emulation_prevention = false;
	enc->shifter = 0;
	enc->bits_in_shifter = 0;
	enc->bits_output = 0;
	enc->num_zeros = 0;
	enc->byte_index = 0;
}

static void radeon_enc_byte_align(struct radeon_encoder *enc)
{
	unsigned int num_padding_zeros = (32 - enc->bits_in_shifter) % 8;

	if (num_padding_zeros > 0)
		radeon_enc_code_fixed_bits(enc, 0, num_padding_zeros);
}

static void radeon_enc_flush_headers(struct radeon_encoder *enc)
{
	if (enc->bits_in_shifter != 0) {
		unsigned char output_byte = (unsigned char)(enc->shifter >> 24);
		radeon_enc_emulation_prevention(enc, output_byte);
		radeon_enc_output_one_byte(enc, output_byte);
		enc->bits_output += enc->bits_in_shifter;
		enc->shifter = 0;
		enc->bits_in_shifter = 0;
		enc->num_zeros = 0;
	}

	if (enc->byte_index > 0) {
		enc->cs->current.cdw++;
		enc->byte_index = 0;
	}
}

static void radeon_enc_code_ue(struct radeon_encoder *enc, unsigned int value)
{
	int x = -1;
	unsigned int ue_code = value + 1;
	value += 1;

	while (value) {
		value = (value >> 1);
		x += 1;
	}

	unsigned int ue_length = (x << 1) + 1;
	radeon_enc_code_fixed_bits(enc, ue_code, ue_length);
}

static void radeon_enc_code_se(struct radeon_encoder *enc, int value)
{
	unsigned int v = 0;

	if (value != 0)
		v = (value < 0 ? ((unsigned int)(0 - value) << 1) : (((unsigned int)(value) << 1) - 1));

	radeon_enc_code_ue(enc, v);
}

static void radeon_enc_session_info(struct radeon_encoder *enc)
{
	unsigned int interface_version = ((RENCODE_FW_INTERFACE_MAJOR_VERSION << RENCODE_IF_MAJOR_VERSION_SHIFT) |
									  (RENCODE_FW_INTERFACE_MINOR_VERSION << RENCODE_IF_MINOR_VERSION_SHIFT));
	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_SESSION_INFO);
	RADEON_ENC_CS(interface_version);
	RADEON_ENC_READWRITE(enc->si->res->buf, enc->si->res->domains, 0x0);
	RADEON_ENC_END();
}

static void radeon_enc_task_info(struct radeon_encoder *enc, bool need_feedback)
{
	enc->enc_pic.task_info.task_id++;

	if (need_feedback)
		enc->enc_pic.task_info.allowed_max_num_feedbacks = 1;
	else
		enc->enc_pic.task_info.allowed_max_num_feedbacks = 0;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_TASK_INFO);
	enc->p_task_size = &enc->cs->current.buf[enc->cs->current.cdw++];
	RADEON_ENC_CS(enc->enc_pic.task_info.task_id);
	RADEON_ENC_CS(enc->enc_pic.task_info.allowed_max_num_feedbacks);
	RADEON_ENC_END();
}

static void radeon_enc_session_init(struct radeon_encoder *enc)
{
	enc->enc_pic.session_init.encode_standard = RENCODE_ENCODE_STANDARD_H264;
	enc->enc_pic.session_init.aligned_picture_width = align(enc->base.width, 16);
	enc->enc_pic.session_init.aligned_picture_height = align(enc->base.height, 16);
	enc->enc_pic.session_init.padding_width = enc->enc_pic.session_init.aligned_picture_width - enc->base.width;
	enc->enc_pic.session_init.padding_height = enc->enc_pic.session_init.aligned_picture_height - enc->base.height;
	enc->enc_pic.session_init.pre_encode_mode = RENCODE_PREENCODE_MODE_NONE;
	enc->enc_pic.session_init.pre_encode_chroma_enabled = false;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_SESSION_INIT);
	RADEON_ENC_CS(enc->enc_pic.session_init.encode_standard);
	RADEON_ENC_CS(enc->enc_pic.session_init.aligned_picture_width);
	RADEON_ENC_CS(enc->enc_pic.session_init.aligned_picture_height);
	RADEON_ENC_CS(enc->enc_pic.session_init.padding_width);
	RADEON_ENC_CS(enc->enc_pic.session_init.padding_height);
	RADEON_ENC_CS(enc->enc_pic.session_init.pre_encode_mode);
	RADEON_ENC_CS(enc->enc_pic.session_init.pre_encode_chroma_enabled);
	RADEON_ENC_END();
}

static void radeon_enc_layer_control(struct radeon_encoder *enc)
{
	enc->enc_pic.layer_ctrl.max_num_temporal_layers = 1;
	enc->enc_pic.layer_ctrl.num_temporal_layers = 1;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_LAYER_CONTROL);
	RADEON_ENC_CS(enc->enc_pic.layer_ctrl.max_num_temporal_layers);
	RADEON_ENC_CS(enc->enc_pic.layer_ctrl.num_temporal_layers);
	RADEON_ENC_END();
}

static void radeon_enc_layer_select(struct radeon_encoder *enc)
{
	enc->enc_pic.layer_sel.temporal_layer_index = 0;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_LAYER_SELECT);
	RADEON_ENC_CS(enc->enc_pic.layer_sel.temporal_layer_index);
	RADEON_ENC_END();
}

static void radeon_enc_slice_control(struct radeon_encoder *enc)
{
	enc->enc_pic.slice_ctrl.slice_control_mode = RENCODE_H264_SLICE_CONTROL_MODE_FIXED_MBS;
	enc->enc_pic.slice_ctrl.num_mbs_per_slice = align(enc->base.width, 16) / 16 * align(enc->base.height, 16) / 16;

	RADEON_ENC_BEGIN(RENCODE_H264_IB_PARAM_SLICE_CONTROL);
	RADEON_ENC_CS(enc->enc_pic.slice_ctrl.slice_control_mode);
	RADEON_ENC_CS(enc->enc_pic.slice_ctrl.num_mbs_per_slice);
	RADEON_ENC_END();
}

static void radeon_enc_spec_misc(struct radeon_encoder *enc)
{
	enc->enc_pic.spec_misc.constrained_intra_pred_flag = 0;
	enc->enc_pic.spec_misc.cabac_enable = 0;
	enc->enc_pic.spec_misc.cabac_init_idc = 0;
	enc->enc_pic.spec_misc.half_pel_enabled = 1;
	enc->enc_pic.spec_misc.quarter_pel_enabled = 1;
	enc->enc_pic.spec_misc.profile_idc = profiles[enc->base.profile - PIPE_VIDEO_PROFILE_MPEG4_AVC_BASELINE];
	enc->enc_pic.spec_misc.level_idc = enc->base.level;

	RADEON_ENC_BEGIN(RENCODE_H264_IB_PARAM_SPEC_MISC);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.constrained_intra_pred_flag);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.cabac_enable);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.cabac_init_idc);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.half_pel_enabled);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.quarter_pel_enabled);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.profile_idc);
	RADEON_ENC_CS(enc->enc_pic.spec_misc.level_idc);
	RADEON_ENC_END();
}

static void radeon_enc_rc_session_init(struct radeon_encoder *enc, struct pipe_h264_enc_picture_desc *pic)
{
	switch(pic->rate_ctrl.rate_ctrl_method) {
		case PIPE_H264_ENC_RATE_CONTROL_METHOD_DISABLE:
			enc->enc_pic.rc_session_init.rate_control_method = RENCODE_RATE_CONTROL_METHOD_NONE;
			break;
		case PIPE_H264_ENC_RATE_CONTROL_METHOD_CONSTANT_SKIP:
		case PIPE_H264_ENC_RATE_CONTROL_METHOD_CONSTANT:
			enc->enc_pic.rc_session_init.rate_control_method = RENCODE_RATE_CONTROL_METHOD_CBR;
			break;
		case PIPE_H264_ENC_RATE_CONTROL_METHOD_VARIABLE_SKIP:
		case PIPE_H264_ENC_RATE_CONTROL_METHOD_VARIABLE:
			enc->enc_pic.rc_session_init.rate_control_method = RENCODE_RATE_CONTROL_METHOD_PEAK_CONSTRAINED_VBR;
			break;
		default:
			enc->enc_pic.rc_session_init.rate_control_method = RENCODE_RATE_CONTROL_METHOD_NONE;
	}

	enc->enc_pic.rc_session_init.vbv_buffer_level = pic->rate_ctrl.vbv_buf_lv;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_RATE_CONTROL_SESSION_INIT);
	RADEON_ENC_CS(enc->enc_pic.rc_session_init.rate_control_method);
	RADEON_ENC_CS(enc->enc_pic.rc_session_init.vbv_buffer_level);
	RADEON_ENC_END();
}

static void radeon_enc_rc_layer_init(struct radeon_encoder *enc, struct pipe_h264_enc_picture_desc *pic)
{
	enc->enc_pic.rc_layer_init.target_bit_rate = pic->rate_ctrl.target_bitrate;
	enc->enc_pic.rc_layer_init.peak_bit_rate = pic->rate_ctrl.peak_bitrate;
	enc->enc_pic.rc_layer_init.frame_rate_num = pic->rate_ctrl.frame_rate_num;
	enc->enc_pic.rc_layer_init.frame_rate_den = pic->rate_ctrl.frame_rate_den;
	enc->enc_pic.rc_layer_init.vbv_buffer_size = pic->rate_ctrl.vbv_buffer_size;
	enc->enc_pic.rc_layer_init.avg_target_bits_per_picture = pic->rate_ctrl.target_bits_picture;
	enc->enc_pic.rc_layer_init.peak_bits_per_picture_integer = pic->rate_ctrl.peak_bits_picture_integer;
	enc->enc_pic.rc_layer_init.peak_bits_per_picture_fractional = pic->rate_ctrl.peak_bits_picture_fraction;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_RATE_CONTROL_LAYER_INIT);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.target_bit_rate);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.peak_bit_rate);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.frame_rate_num);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.frame_rate_den);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.vbv_buffer_size);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.avg_target_bits_per_picture);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.peak_bits_per_picture_integer);
	RADEON_ENC_CS(enc->enc_pic.rc_layer_init.peak_bits_per_picture_fractional);
	RADEON_ENC_END();
}

static void radeon_enc_deblocking_filter_h264(struct radeon_encoder *enc)
{
	enc->enc_pic.h264_deblock.disable_deblocking_filter_idc = 0;
	enc->enc_pic.h264_deblock.alpha_c0_offset_div2 = 0;
	enc->enc_pic.h264_deblock.beta_offset_div2 = 0;
	enc->enc_pic.h264_deblock.cb_qp_offset = 0;
	enc->enc_pic.h264_deblock.cr_qp_offset = 0;

	RADEON_ENC_BEGIN(RENCODE_H264_IB_PARAM_DEBLOCKING_FILTER);
	RADEON_ENC_CS(enc->enc_pic.h264_deblock.disable_deblocking_filter_idc);
	RADEON_ENC_CS(enc->enc_pic.h264_deblock.alpha_c0_offset_div2);
	RADEON_ENC_CS(enc->enc_pic.h264_deblock.beta_offset_div2);
	RADEON_ENC_CS(enc->enc_pic.h264_deblock.cb_qp_offset);
	RADEON_ENC_CS(enc->enc_pic.h264_deblock.cr_qp_offset);
	RADEON_ENC_END();
}

static void radeon_enc_quality_params(struct radeon_encoder *enc)
{
	enc->enc_pic.quality_params.vbaq_mode = 0;
	enc->enc_pic.quality_params.scene_change_sensitivity = 0;
	enc->enc_pic.quality_params.scene_change_min_idr_interval = 0;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_QUALITY_PARAMS);
	RADEON_ENC_CS(enc->enc_pic.quality_params.vbaq_mode);
	RADEON_ENC_CS(enc->enc_pic.quality_params.scene_change_sensitivity);
	RADEON_ENC_CS(enc->enc_pic.quality_params.scene_change_min_idr_interval);
	RADEON_ENC_END();
}

static void radeon_enc_nalu_sps(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_DIRECT_OUTPUT_NALU);
	RADEON_ENC_CS(RENCODE_DIRECT_OUTPUT_NALU_TYPE_SPS);
	uint32_t *size_in_bytes = &enc->cs->current.buf[enc->cs->current.cdw++];
	radeon_enc_reset(enc);
	radeon_enc_set_emulation_prevention(enc, false);
	radeon_enc_code_fixed_bits(enc, 0x00000001, 32);
	radeon_enc_code_fixed_bits(enc, 0x67, 8);
	radeon_enc_byte_align(enc);
	radeon_enc_set_emulation_prevention(enc, true);
	radeon_enc_code_fixed_bits(enc, enc->enc_pic.spec_misc.profile_idc, 8);
	radeon_enc_code_fixed_bits(enc, 0x44, 8); //hardcode to constrained baseline
	radeon_enc_code_fixed_bits(enc, enc->enc_pic.spec_misc.level_idc, 8);
	radeon_enc_code_ue(enc, 0x0);

	if(enc->enc_pic.spec_misc.profile_idc == 100 || enc->enc_pic.spec_misc.profile_idc == 110 || enc->enc_pic.spec_misc.profile_idc == 122 ||
			enc->enc_pic.spec_misc.profile_idc == 244 || enc->enc_pic.spec_misc.profile_idc == 44 || enc->enc_pic.spec_misc.profile_idc == 83 ||
			enc->enc_pic.spec_misc.profile_idc == 86 || enc->enc_pic.spec_misc.profile_idc == 118 || enc->enc_pic.spec_misc.profile_idc == 128 ||
			enc->enc_pic.spec_misc.profile_idc == 138) {
		radeon_enc_code_ue(enc, 0x1);
		radeon_enc_code_ue(enc, 0x0);
		radeon_enc_code_ue(enc, 0x0);
		radeon_enc_code_fixed_bits(enc, 0x0, 2);
	}

	radeon_enc_code_ue(enc, 1);
	radeon_enc_code_ue(enc, enc->enc_pic.pic_order_cnt_type);

	if (enc->enc_pic.pic_order_cnt_type == 0)
		radeon_enc_code_ue(enc, 1);

	radeon_enc_code_ue(enc, (enc->base.max_references + 1));
	radeon_enc_code_fixed_bits(enc, enc->enc_pic.layer_ctrl.max_num_temporal_layers > 1 ? 0x1 : 0x0, 1);
	radeon_enc_code_ue(enc, (enc->enc_pic.session_init.aligned_picture_width / 16 - 1));
	radeon_enc_code_ue(enc, (enc->enc_pic.session_init.aligned_picture_height / 16 - 1));
	bool progressive_only = true;
	radeon_enc_code_fixed_bits(enc, progressive_only ? 0x1 : 0x0, 1);

	if (!progressive_only)
		radeon_enc_code_fixed_bits(enc, 0x0, 1);

	radeon_enc_code_fixed_bits(enc, 0x1, 1);

	if ((enc->enc_pic.crop_left != 0) || (enc->enc_pic.crop_right != 0) ||
			(enc->enc_pic.crop_top != 0) || (enc->enc_pic.crop_bottom != 0)) {
		radeon_enc_code_fixed_bits(enc, 0x1, 1);
		radeon_enc_code_ue(enc, enc->enc_pic.crop_left);
		radeon_enc_code_ue(enc, enc->enc_pic.crop_right);
		radeon_enc_code_ue(enc, enc->enc_pic.crop_top);
		radeon_enc_code_ue(enc, enc->enc_pic.crop_bottom);
	} else
		radeon_enc_code_fixed_bits(enc, 0x0, 1);

	radeon_enc_code_fixed_bits(enc, 0x1, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x1, 1);
	radeon_enc_code_fixed_bits(enc, 0x1, 1);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, 16);
	radeon_enc_code_ue(enc, 16);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, (enc->base.max_references + 1));

	radeon_enc_code_fixed_bits(enc, 0x1, 1);

	radeon_enc_byte_align(enc);
	radeon_enc_flush_headers(enc);
	*size_in_bytes = (enc->bits_output + 7) / 8;
	RADEON_ENC_END();
}

static void radeon_enc_nalu_pps(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_DIRECT_OUTPUT_NALU);
	RADEON_ENC_CS(RENCODE_DIRECT_OUTPUT_NALU_TYPE_PPS);
	uint32_t *size_in_bytes = &enc->cs->current.buf[enc->cs->current.cdw++];
	radeon_enc_reset(enc);
	radeon_enc_set_emulation_prevention(enc, false);
	radeon_enc_code_fixed_bits(enc, 0x00000001, 32);
	radeon_enc_code_fixed_bits(enc, 0x68, 8);
	radeon_enc_byte_align(enc);
	radeon_enc_set_emulation_prevention(enc, true);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_fixed_bits(enc, (enc->enc_pic.spec_misc.cabac_enable ? 0x1 : 0x0), 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 2);
	radeon_enc_code_se(enc, 0x0);
	radeon_enc_code_se(enc, 0x0);
	radeon_enc_code_se(enc, 0x0);
	radeon_enc_code_fixed_bits(enc, 0x1, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);
	radeon_enc_code_fixed_bits(enc, 0x0, 1);

	radeon_enc_code_fixed_bits(enc, 0x1, 1);

	radeon_enc_byte_align(enc);
	radeon_enc_flush_headers(enc);
	*size_in_bytes = (enc->bits_output + 7) / 8;
	RADEON_ENC_END();
}

static void radeon_enc_slice_header(struct radeon_encoder *enc)
{
	uint32_t instruction[RENCODE_SLICE_HEADER_TEMPLATE_MAX_NUM_INSTRUCTIONS] = {0};
	uint32_t num_bits[RENCODE_SLICE_HEADER_TEMPLATE_MAX_NUM_INSTRUCTIONS] = {0};
	unsigned int inst_index = 0;
	unsigned int bit_index = 0;
	unsigned int bits_copied = 0;
	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_SLICE_HEADER);
	radeon_enc_reset(enc);
	radeon_enc_set_emulation_prevention(enc, false);

	if (enc->enc_pic.is_idr)
		radeon_enc_code_fixed_bits(enc, 0x65, 8);
	else if (enc->enc_pic.not_referenced)
		radeon_enc_code_fixed_bits(enc, 0x01, 8);
	else
		radeon_enc_code_fixed_bits(enc, 0x41, 8);

	radeon_enc_flush_headers(enc);
	bit_index ++;
	instruction[inst_index] = RENCODE_HEADER_INSTRUCTION_COPY;
	num_bits[inst_index] = enc->bits_output - bits_copied;
	bits_copied = enc->bits_output;
	inst_index++;

	instruction[inst_index] = RENCODE_H264_HEADER_INSTRUCTION_FIRST_MB;
	inst_index++;

	switch(enc->enc_pic.picture_type) {
		case PIPE_H264_ENC_PICTURE_TYPE_I:
		case PIPE_H264_ENC_PICTURE_TYPE_IDR:
			radeon_enc_code_fixed_bits(enc, 0x08, 7);
			break;
		case PIPE_H264_ENC_PICTURE_TYPE_P:
		case PIPE_H264_ENC_PICTURE_TYPE_SKIP:
			radeon_enc_code_fixed_bits(enc, 0x06, 5);
			break;
		case PIPE_H264_ENC_PICTURE_TYPE_B:
			radeon_enc_code_fixed_bits(enc, 0x07, 5);
			break;
		default:
			radeon_enc_code_fixed_bits(enc, 0x08, 7);
	}

	radeon_enc_code_ue(enc, 0x0);
	radeon_enc_code_fixed_bits(enc, enc->enc_pic.frame_num % 32, 5);

	if (enc->enc_pic.h264_enc_params.input_picture_structure != RENCODE_H264_PICTURE_STRUCTURE_FRAME) {
		radeon_enc_code_fixed_bits(enc, 0x1, 1);
		radeon_enc_code_fixed_bits(enc, enc->enc_pic.h264_enc_params.input_picture_structure == RENCODE_H264_PICTURE_STRUCTURE_BOTTOM_FIELD ? 1 : 0, 1);
	}

	if (enc->enc_pic.is_idr)
		radeon_enc_code_ue(enc, enc->enc_pic.is_even_frame);

	enc->enc_pic.is_even_frame = !enc->enc_pic.is_even_frame;

	if (enc->enc_pic.pic_order_cnt_type == 0)
		radeon_enc_code_fixed_bits(enc, enc->enc_pic.pic_order_cnt % 32, 5);

	if (enc->enc_pic.picture_type != PIPE_H264_ENC_PICTURE_TYPE_IDR) {
		radeon_enc_code_fixed_bits(enc, 0x0, 1);

		if (enc->enc_pic.frame_num - enc->enc_pic.ref_idx_l0 > 1) {
			radeon_enc_code_fixed_bits(enc, 0x1, 1);
			radeon_enc_code_ue(enc, 0x0);
			radeon_enc_code_ue(enc, (enc->enc_pic.frame_num - enc->enc_pic.ref_idx_l0 - 1));
			radeon_enc_code_ue(enc, 0x3);
		} else
			radeon_enc_code_fixed_bits(enc, 0x0, 1);
	}

	if (enc->enc_pic.is_idr) {
		radeon_enc_code_fixed_bits(enc, 0x0, 1);
		radeon_enc_code_fixed_bits(enc, 0x0, 1);
	} else
		radeon_enc_code_fixed_bits(enc, 0x0, 1);

	if ((enc->enc_pic.picture_type != PIPE_H264_ENC_PICTURE_TYPE_IDR) && (enc->enc_pic.spec_misc.cabac_enable))
		radeon_enc_code_ue(enc, enc->enc_pic.spec_misc.cabac_init_idc);

	radeon_enc_flush_headers(enc);
	bit_index ++;
	instruction[inst_index] = RENCODE_HEADER_INSTRUCTION_COPY;
	num_bits[inst_index] = enc->bits_output - bits_copied;
	bits_copied = enc->bits_output;
	inst_index++;

	instruction[inst_index] = RENCODE_H264_HEADER_INSTRUCTION_SLICE_QP_DELTA;
	inst_index++;

	radeon_enc_code_ue(enc, enc->enc_pic.h264_deblock.disable_deblocking_filter_idc ? 1: 0);

	if (!enc->enc_pic.h264_deblock.disable_deblocking_filter_idc) {
		radeon_enc_code_se(enc, enc->enc_pic.h264_deblock.alpha_c0_offset_div2);
		radeon_enc_code_se(enc, enc->enc_pic.h264_deblock.beta_offset_div2);
	}

	radeon_enc_flush_headers(enc);
	bit_index ++;
	instruction[inst_index] = RENCODE_HEADER_INSTRUCTION_COPY;
	num_bits[inst_index] = enc->bits_output - bits_copied;
	bits_copied = enc->bits_output;
	inst_index++;

	instruction[inst_index] = RENCODE_HEADER_INSTRUCTION_END;

	for (int i = bit_index; i < RENCODE_SLICE_HEADER_TEMPLATE_MAX_TEMPLATE_SIZE_IN_DWORDS; i++)
		RADEON_ENC_CS(0x00000000);

	for (int j = 0; j < RENCODE_SLICE_HEADER_TEMPLATE_MAX_NUM_INSTRUCTIONS; j++) {
		RADEON_ENC_CS(instruction[j]);
		RADEON_ENC_CS(num_bits[j]);
	}

	RADEON_ENC_END();
}

static void radeon_enc_ctx(struct radeon_encoder *enc)
{
	enc->enc_pic.ctx_buf.swizzle_mode = 0;
	enc->enc_pic.ctx_buf.rec_luma_pitch = align(enc->base.width, enc->alignment);
	enc->enc_pic.ctx_buf.rec_chroma_pitch = align(enc->base.width, enc->alignment);
	enc->enc_pic.ctx_buf.num_reconstructed_pictures = 2;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_ENCODE_CONTEXT_BUFFER);
	RADEON_ENC_READWRITE(enc->cpb.res->buf, enc->cpb.res->domains, 0);
	RADEON_ENC_CS(enc->enc_pic.ctx_buf.swizzle_mode);
	RADEON_ENC_CS(enc->enc_pic.ctx_buf.rec_luma_pitch);
	RADEON_ENC_CS(enc->enc_pic.ctx_buf.rec_chroma_pitch);
	RADEON_ENC_CS(enc->enc_pic.ctx_buf.num_reconstructed_pictures);
	/* reconstructed_picture_1_luma_offset */
	RADEON_ENC_CS(0x00000000);
	/* reconstructed_picture_1_chroma_offset */
	RADEON_ENC_CS(align(enc->base.width, enc->alignment) * align(enc->base.height, 16));
	/* reconstructed_picture_2_luma_offset */
	RADEON_ENC_CS(align(enc->base.width, enc->alignment) * align(enc->base.height, 16) * 3 / 2);
	/* reconstructed_picture_2_chroma_offset */
	RADEON_ENC_CS(align(enc->base.width, enc->alignment) * align(enc->base.height, 16) * 5 / 2);

	for (int i = 0; i < 136 ; i++)
		RADEON_ENC_CS(0x00000000);

	RADEON_ENC_END();
}

static void radeon_enc_bitstream(struct radeon_encoder *enc)
{
	enc->enc_pic.bit_buf.mode = RENCODE_REC_SWIZZLE_MODE_LINEAR;
	enc->enc_pic.bit_buf.video_bitstream_buffer_size = enc->bs_size;
	enc->enc_pic.bit_buf.video_bitstream_data_offset = 0;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_VIDEO_BITSTREAM_BUFFER);
	RADEON_ENC_CS(enc->enc_pic.bit_buf.mode);
	RADEON_ENC_WRITE(enc->bs_handle, RADEON_DOMAIN_GTT, 0);
	RADEON_ENC_CS(enc->enc_pic.bit_buf.video_bitstream_buffer_size);
	RADEON_ENC_CS(enc->enc_pic.bit_buf.video_bitstream_data_offset);
	RADEON_ENC_END();
}

static void radeon_enc_feedback(struct radeon_encoder *enc)
{
	enc->enc_pic.fb_buf.mode = RENCODE_FEEDBACK_BUFFER_MODE_LINEAR;
	enc->enc_pic.fb_buf.feedback_buffer_size = 16;
	enc->enc_pic.fb_buf.feedback_data_size = 40;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_FEEDBACK_BUFFER);
	RADEON_ENC_CS(enc->enc_pic.fb_buf.mode);
	RADEON_ENC_WRITE(enc->fb->res->buf, enc->fb->res->domains, 0x0);
	RADEON_ENC_CS(enc->enc_pic.fb_buf.feedback_buffer_size);
	RADEON_ENC_CS(enc->enc_pic.fb_buf.feedback_data_size);
	RADEON_ENC_END();
}

static void radeon_enc_intra_refresh(struct radeon_encoder *enc)
{
	enc->enc_pic.intra_ref.intra_refresh_mode = RENCODE_INTRA_REFRESH_MODE_NONE;
	enc->enc_pic.intra_ref.offset = 0;
	enc->enc_pic.intra_ref.region_size = 0;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_INTRA_REFRESH);
	RADEON_ENC_CS(enc->enc_pic.intra_ref.intra_refresh_mode);
	RADEON_ENC_CS(enc->enc_pic.intra_ref.offset);
	RADEON_ENC_CS(enc->enc_pic.intra_ref.region_size);
	RADEON_ENC_END();
}

static void radeon_enc_rc_per_pic(struct radeon_encoder *enc, struct pipe_h264_enc_picture_desc *pic)
{
	enc->enc_pic.rc_per_pic.qp = pic->quant_i_frames;
	enc->enc_pic.rc_per_pic.min_qp_app = 0;
	enc->enc_pic.rc_per_pic.max_qp_app = 51;
	enc->enc_pic.rc_per_pic.max_au_size = 0;
	enc->enc_pic.rc_per_pic.enabled_filler_data = pic->rate_ctrl.fill_data_enable;
	enc->enc_pic.rc_per_pic.skip_frame_enable = false;
	enc->enc_pic.rc_per_pic.enforce_hrd = pic->rate_ctrl.enforce_hrd;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_RATE_CONTROL_PER_PICTURE);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.qp);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.min_qp_app);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.max_qp_app);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.max_au_size);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.enabled_filler_data);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.skip_frame_enable);
	RADEON_ENC_CS(enc->enc_pic.rc_per_pic.enforce_hrd);
	RADEON_ENC_END();
}

static void radeon_enc_encode_params(struct radeon_encoder *enc)
{
	switch(enc->enc_pic.picture_type) {
		case PIPE_H264_ENC_PICTURE_TYPE_I:
		case PIPE_H264_ENC_PICTURE_TYPE_IDR:
			enc->enc_pic.enc_params.pic_type = RENCODE_PICTURE_TYPE_I;
			break;
		case PIPE_H264_ENC_PICTURE_TYPE_P:
			enc->enc_pic.enc_params.pic_type = RENCODE_PICTURE_TYPE_P;
			break;
		case PIPE_H264_ENC_PICTURE_TYPE_SKIP:
			enc->enc_pic.enc_params.pic_type = RENCODE_PICTURE_TYPE_P_SKIP;
			break;
		case PIPE_H264_ENC_PICTURE_TYPE_B:
			enc->enc_pic.enc_params.pic_type = RENCODE_PICTURE_TYPE_B;
			break;
		default:
			enc->enc_pic.enc_params.pic_type = RENCODE_PICTURE_TYPE_I;
	}

	enc->enc_pic.enc_params.allowed_max_bitstream_size = enc->bs_size;
	enc->enc_pic.enc_params.input_pic_luma_pitch = enc->luma->u.gfx9.surf_pitch;
	enc->enc_pic.enc_params.input_pic_chroma_pitch = enc->chroma->u.gfx9.surf_pitch;
	enc->enc_pic.enc_params.input_pic_swizzle_mode = RENCODE_INPUT_SWIZZLE_MODE_LINEAR;

	if(enc->enc_pic.picture_type == PIPE_H264_ENC_PICTURE_TYPE_IDR)
		enc->enc_pic.enc_params.reference_picture_index = 0xFFFFFFFF;
	else
		enc->enc_pic.enc_params.reference_picture_index = (enc->enc_pic.frame_num - 1) % 2;

	enc->enc_pic.enc_params.reconstructed_picture_index = enc->enc_pic.frame_num % 2;

	RADEON_ENC_BEGIN(RENCODE_IB_PARAM_ENCODE_PARAMS);
	RADEON_ENC_CS(enc->enc_pic.enc_params.pic_type);
	RADEON_ENC_CS(enc->enc_pic.enc_params.allowed_max_bitstream_size);
	RADEON_ENC_READ(enc->handle, RADEON_DOMAIN_VRAM, enc->luma->u.gfx9.surf_offset);
	RADEON_ENC_READ(enc->handle, RADEON_DOMAIN_VRAM, enc->chroma->u.gfx9.surf_offset);
	RADEON_ENC_CS(enc->enc_pic.enc_params.input_pic_luma_pitch);
	RADEON_ENC_CS(enc->enc_pic.enc_params.input_pic_chroma_pitch);
	RADEON_ENC_CS(enc->enc_pic.enc_params.input_pic_swizzle_mode);
	RADEON_ENC_CS(enc->enc_pic.enc_params.reference_picture_index);
	RADEON_ENC_CS(enc->enc_pic.enc_params.reconstructed_picture_index);
	RADEON_ENC_END();
}
static void radeon_enc_encode_params_h264(struct radeon_encoder *enc)
{
	enc->enc_pic.h264_enc_params.input_picture_structure = RENCODE_H264_PICTURE_STRUCTURE_FRAME;
	enc->enc_pic.h264_enc_params.interlaced_mode = RENCODE_H264_INTERLACING_MODE_PROGRESSIVE;
	enc->enc_pic.h264_enc_params.reference_picture_structure = RENCODE_H264_PICTURE_STRUCTURE_FRAME;
	enc->enc_pic.h264_enc_params.reference_picture1_index = 0xFFFFFFFF;

	RADEON_ENC_BEGIN(RENCODE_H264_IB_PARAM_ENCODE_PARAMS);
	RADEON_ENC_CS(enc->enc_pic.h264_enc_params.input_picture_structure);
	RADEON_ENC_CS(enc->enc_pic.h264_enc_params.interlaced_mode);
	RADEON_ENC_CS(enc->enc_pic.h264_enc_params.reference_picture_structure);
	RADEON_ENC_CS(enc->enc_pic.h264_enc_params.reference_picture1_index);
	RADEON_ENC_END();
}

static void radeon_enc_op_init(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_INITIALIZE);
	RADEON_ENC_END();
}

static void radeon_enc_op_close(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_CLOSE_SESSION);
	RADEON_ENC_END();
}

static void radeon_enc_op_enc(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_ENCODE);
	RADEON_ENC_END();
}

static void radeon_enc_op_init_rc(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_INIT_RC);
	RADEON_ENC_END();
}

static void radeon_enc_op_init_rc_vbv(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_INIT_RC_VBV_BUFFER_LEVEL);
	RADEON_ENC_END();
}

static void radeon_enc_op_speed(struct radeon_encoder *enc)
{
	RADEON_ENC_BEGIN(RENCODE_IB_OP_SET_SPEED_ENCODING_MODE);
	RADEON_ENC_END();
}

static void begin(struct radeon_encoder *enc, struct pipe_h264_enc_picture_desc *pic)
{
	radeon_enc_session_info(enc);
	enc->total_task_size = 0;
	radeon_enc_task_info(enc, enc->need_feedback);
	radeon_enc_op_init(enc);
	radeon_enc_session_init(enc);
	radeon_enc_layer_control(enc);
	radeon_enc_slice_control(enc);
	radeon_enc_spec_misc(enc);
	radeon_enc_rc_session_init(enc, pic);
	radeon_enc_deblocking_filter_h264(enc);
	radeon_enc_quality_params(enc);
	radeon_enc_layer_select(enc);
	radeon_enc_rc_layer_init(enc, pic);
	radeon_enc_layer_select(enc);
	radeon_enc_rc_per_pic(enc, pic);
	radeon_enc_op_init_rc(enc);
	radeon_enc_op_init_rc_vbv(enc);
	*enc->p_task_size = (enc->total_task_size);
}

static void encode(struct radeon_encoder *enc)
{
	radeon_enc_session_info(enc);
	enc->total_task_size = 0;
	radeon_enc_task_info(enc, enc->need_feedback);

	if (enc->enc_pic.is_idr) {
		radeon_enc_nalu_sps(enc);
		radeon_enc_nalu_pps(enc);
	}

	radeon_enc_slice_header(enc);
	radeon_enc_ctx(enc);
	radeon_enc_bitstream(enc);
	radeon_enc_feedback(enc);
	radeon_enc_intra_refresh(enc);
	radeon_enc_encode_params(enc);
	radeon_enc_encode_params_h264(enc);
	radeon_enc_op_speed(enc);
	radeon_enc_op_enc(enc);
	*enc->p_task_size = (enc->total_task_size);
}

static void destroy(struct radeon_encoder *enc)
{
	radeon_enc_session_info(enc);
	enc->total_task_size = 0;
	radeon_enc_task_info(enc, enc->need_feedback);
	radeon_enc_op_close(enc);
	*enc->p_task_size = (enc->total_task_size);
}

void radeon_enc_1_2_init(struct radeon_encoder *enc)
{
	enc->begin = begin;
	enc->encode = encode;
	enc->destroy = destroy;
}
