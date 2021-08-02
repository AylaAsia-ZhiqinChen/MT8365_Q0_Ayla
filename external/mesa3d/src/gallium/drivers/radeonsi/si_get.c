/*
 * Copyright 2017 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "si_pipe.h"
#include "radeon/radeon_video.h"
#include "radeon/radeon_vce.h"
#include "ac_llvm_util.h"
#include "vl/vl_decoder.h"
#include "vl/vl_video_buffer.h"
#include "util/u_video.h"
#include "compiler/nir/nir.h"

#include <sys/utsname.h>

static const char *si_get_vendor(struct pipe_screen *pscreen)
{
	/* Don't change this. Games such as Alien Isolation are broken if this
	 * returns "Advanced Micro Devices, Inc."
	 */
	return "X.Org";
}

static const char *si_get_device_vendor(struct pipe_screen *pscreen)
{
	return "AMD";
}

static const char *si_get_marketing_name(struct radeon_winsys *ws)
{
	if (!ws->get_chip_name)
		return NULL;
	return ws->get_chip_name(ws);
}

const char *si_get_family_name(const struct si_screen *sscreen)
{
	switch (sscreen->info.family) {
	case CHIP_TAHITI: return "AMD TAHITI";
	case CHIP_PITCAIRN: return "AMD PITCAIRN";
	case CHIP_VERDE: return "AMD CAPE VERDE";
	case CHIP_OLAND: return "AMD OLAND";
	case CHIP_HAINAN: return "AMD HAINAN";
	case CHIP_BONAIRE: return "AMD BONAIRE";
	case CHIP_KAVERI: return "AMD KAVERI";
	case CHIP_KABINI: return "AMD KABINI";
	case CHIP_HAWAII: return "AMD HAWAII";
	case CHIP_MULLINS: return "AMD MULLINS";
	case CHIP_TONGA: return "AMD TONGA";
	case CHIP_ICELAND: return "AMD ICELAND";
	case CHIP_CARRIZO: return "AMD CARRIZO";
	case CHIP_FIJI: return "AMD FIJI";
	case CHIP_POLARIS10: return "AMD POLARIS10";
	case CHIP_POLARIS11: return "AMD POLARIS11";
	case CHIP_POLARIS12: return "AMD POLARIS12";
	case CHIP_STONEY: return "AMD STONEY";
	case CHIP_VEGA10: return "AMD VEGA10";
	case CHIP_RAVEN: return "AMD RAVEN";
	default: return "AMD unknown";
	}
}

static bool si_have_tgsi_compute(struct si_screen *sscreen)
{
	/* Old kernels disallowed some register writes for SI
	 * that are used for indirect dispatches. */
	return (sscreen->info.chip_class >= CIK ||
		sscreen->info.drm_major == 3 ||
		(sscreen->info.drm_major == 2 &&
		 sscreen->info.drm_minor >= 45));
}

static int si_get_param(struct pipe_screen *pscreen, enum pipe_cap param)
{
	struct si_screen *sscreen = (struct si_screen *)pscreen;

	switch (param) {
	/* Supported features (boolean caps). */
	case PIPE_CAP_ACCELERATED:
	case PIPE_CAP_MAX_DUAL_SOURCE_RENDER_TARGETS:
	case PIPE_CAP_ANISOTROPIC_FILTER:
	case PIPE_CAP_POINT_SPRITE:
	case PIPE_CAP_OCCLUSION_QUERY:
	case PIPE_CAP_TEXTURE_MIRROR_CLAMP:
	case PIPE_CAP_BLEND_EQUATION_SEPARATE:
	case PIPE_CAP_TEXTURE_SWIZZLE:
	case PIPE_CAP_DEPTH_CLIP_DISABLE:
	case PIPE_CAP_SHADER_STENCIL_EXPORT:
	case PIPE_CAP_VERTEX_ELEMENT_INSTANCE_DIVISOR:
	case PIPE_CAP_MIXED_COLORBUFFER_FORMATS:
	case PIPE_CAP_TGSI_FS_COORD_ORIGIN_UPPER_LEFT:
	case PIPE_CAP_TGSI_FS_COORD_PIXEL_CENTER_HALF_INTEGER:
	case PIPE_CAP_TGSI_FS_COORD_PIXEL_CENTER_INTEGER:
	case PIPE_CAP_SM3:
	case PIPE_CAP_SEAMLESS_CUBE_MAP:
	case PIPE_CAP_PRIMITIVE_RESTART:
	case PIPE_CAP_CONDITIONAL_RENDER:
	case PIPE_CAP_TEXTURE_BARRIER:
	case PIPE_CAP_INDEP_BLEND_ENABLE:
	case PIPE_CAP_INDEP_BLEND_FUNC:
	case PIPE_CAP_SEAMLESS_CUBE_MAP_PER_TEXTURE:
	case PIPE_CAP_VERTEX_COLOR_UNCLAMPED:
	case PIPE_CAP_START_INSTANCE:
	case PIPE_CAP_NPOT_TEXTURES:
	case PIPE_CAP_MIXED_FRAMEBUFFER_SIZES:
	case PIPE_CAP_MIXED_COLOR_DEPTH_BITS:
	case PIPE_CAP_VERTEX_COLOR_CLAMPED:
	case PIPE_CAP_FRAGMENT_COLOR_CLAMPED:
	case PIPE_CAP_PREFER_BLIT_BASED_TEXTURE_TRANSFER:
	case PIPE_CAP_TGSI_INSTANCEID:
	case PIPE_CAP_COMPUTE:
	case PIPE_CAP_TEXTURE_BUFFER_OBJECTS:
	case PIPE_CAP_TGSI_VS_LAYER_VIEWPORT:
	case PIPE_CAP_QUERY_PIPELINE_STATISTICS:
	case PIPE_CAP_BUFFER_MAP_PERSISTENT_COHERENT:
	case PIPE_CAP_CUBE_MAP_ARRAY:
	case PIPE_CAP_SAMPLE_SHADING:
	case PIPE_CAP_DRAW_INDIRECT:
	case PIPE_CAP_CLIP_HALFZ:
	case PIPE_CAP_TGSI_VS_WINDOW_SPACE_POSITION:
	case PIPE_CAP_POLYGON_OFFSET_CLAMP:
	case PIPE_CAP_MULTISAMPLE_Z_RESOLVE:
	case PIPE_CAP_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION:
	case PIPE_CAP_TGSI_TEXCOORD:
	case PIPE_CAP_TGSI_FS_FINE_DERIVATIVE:
	case PIPE_CAP_CONDITIONAL_RENDER_INVERTED:
	case PIPE_CAP_TEXTURE_FLOAT_LINEAR:
	case PIPE_CAP_TEXTURE_HALF_FLOAT_LINEAR:
	case PIPE_CAP_SHAREABLE_SHADERS:
	case PIPE_CAP_DEPTH_BOUNDS_TEST:
	case PIPE_CAP_SAMPLER_VIEW_TARGET:
	case PIPE_CAP_TEXTURE_QUERY_LOD:
	case PIPE_CAP_TEXTURE_GATHER_SM5:
	case PIPE_CAP_TGSI_TXQS:
	case PIPE_CAP_FORCE_PERSAMPLE_INTERP:
	case PIPE_CAP_COPY_BETWEEN_COMPRESSED_AND_PLAIN_FORMATS:
	case PIPE_CAP_TGSI_FS_POSITION_IS_SYSVAL:
	case PIPE_CAP_TGSI_FS_FACE_IS_INTEGER_SYSVAL:
	case PIPE_CAP_INVALIDATE_BUFFER:
	case PIPE_CAP_SURFACE_REINTERPRET_BLOCKS:
	case PIPE_CAP_QUERY_MEMORY_INFO:
	case PIPE_CAP_TGSI_PACK_HALF_FLOAT:
	case PIPE_CAP_FRAMEBUFFER_NO_ATTACHMENT:
	case PIPE_CAP_ROBUST_BUFFER_ACCESS_BEHAVIOR:
	case PIPE_CAP_GENERATE_MIPMAP:
	case PIPE_CAP_POLYGON_OFFSET_UNITS_UNSCALED:
	case PIPE_CAP_STRING_MARKER:
	case PIPE_CAP_CLEAR_TEXTURE:
	case PIPE_CAP_CULL_DISTANCE:
	case PIPE_CAP_TGSI_ARRAY_COMPONENTS:
	case PIPE_CAP_TGSI_CAN_READ_OUTPUTS:
	case PIPE_CAP_GLSL_OPTIMIZE_CONSERVATIVELY:
	case PIPE_CAP_STREAM_OUTPUT_PAUSE_RESUME:
	case PIPE_CAP_STREAM_OUTPUT_INTERLEAVE_BUFFERS:
	case PIPE_CAP_DOUBLES:
	case PIPE_CAP_TGSI_TEX_TXF_LZ:
	case PIPE_CAP_TGSI_TES_LAYER_VIEWPORT:
	case PIPE_CAP_BINDLESS_TEXTURE:
	case PIPE_CAP_QUERY_TIMESTAMP:
	case PIPE_CAP_QUERY_TIME_ELAPSED:
	case PIPE_CAP_NIR_SAMPLERS_AS_DEREF:
	case PIPE_CAP_QUERY_SO_OVERFLOW:
	case PIPE_CAP_MEMOBJ:
	case PIPE_CAP_LOAD_CONSTBUF:
	case PIPE_CAP_INT64:
	case PIPE_CAP_INT64_DIVMOD:
	case PIPE_CAP_TGSI_CLOCK:
	case PIPE_CAP_CAN_BIND_CONST_BUFFER_AS_VERTEX:
	case PIPE_CAP_ALLOW_MAPPED_BUFFERS_DURING_EXECUTION:
	case PIPE_CAP_TGSI_ANY_REG_AS_ADDRESS:
	case PIPE_CAP_SIGNED_VERTEX_BUFFER_OFFSET:
		return 1;

	case PIPE_CAP_TGSI_VOTE:
		return HAVE_LLVM >= 0x0400;

	case PIPE_CAP_TGSI_BALLOT:
		return HAVE_LLVM >= 0x0500;

	case PIPE_CAP_RESOURCE_FROM_USER_MEMORY:
		return !SI_BIG_ENDIAN && sscreen->info.has_userptr;

	case PIPE_CAP_DEVICE_RESET_STATUS_QUERY:
		return (sscreen->info.drm_major == 2 &&
			sscreen->info.drm_minor >= 43) ||
		       sscreen->info.drm_major == 3;

	case PIPE_CAP_TEXTURE_MULTISAMPLE:
		/* 2D tiling on CIK is supported since DRM 2.35.0 */
		return sscreen->info.chip_class < CIK ||
		       (sscreen->info.drm_major == 2 &&
			sscreen->info.drm_minor >= 35) ||
		       sscreen->info.drm_major == 3;

        case PIPE_CAP_MIN_MAP_BUFFER_ALIGNMENT:
                return R600_MAP_BUFFER_ALIGNMENT;

	case PIPE_CAP_CONSTANT_BUFFER_OFFSET_ALIGNMENT:
	case PIPE_CAP_TEXTURE_BUFFER_OFFSET_ALIGNMENT:
	case PIPE_CAP_MAX_TEXTURE_GATHER_COMPONENTS:
	case PIPE_CAP_MAX_STREAM_OUTPUT_BUFFERS:
	case PIPE_CAP_MAX_VERTEX_STREAMS:
	case PIPE_CAP_SHADER_BUFFER_OFFSET_ALIGNMENT:
		return 4;

	case PIPE_CAP_GLSL_FEATURE_LEVEL:
		if (si_have_tgsi_compute(sscreen))
			return 450;
		return 420;

	case PIPE_CAP_MAX_TEXTURE_BUFFER_SIZE:
		return MIN2(sscreen->info.max_alloc_size, INT_MAX);

	case PIPE_CAP_VERTEX_BUFFER_OFFSET_4BYTE_ALIGNED_ONLY:
	case PIPE_CAP_VERTEX_BUFFER_STRIDE_4BYTE_ALIGNED_ONLY:
	case PIPE_CAP_VERTEX_ELEMENT_SRC_OFFSET_4BYTE_ALIGNED_ONLY:
		/* SI doesn't support unaligned loads.
		 * CIK needs DRM 2.50.0 on radeon. */
		return sscreen->info.chip_class == SI ||
		       (sscreen->info.drm_major == 2 &&
			sscreen->info.drm_minor < 50);

	case PIPE_CAP_SPARSE_BUFFER_PAGE_SIZE:
		/* TODO: GFX9 hangs. */
		if (sscreen->info.chip_class >= GFX9)
			return 0;
		/* Disable on SI due to VM faults in CP DMA. Enable once these
		 * faults are mitigated in software.
		 */
		if (sscreen->info.chip_class >= CIK &&
		    sscreen->info.drm_major == 3 &&
		    sscreen->info.drm_minor >= 13)
			return RADEON_SPARSE_PAGE_SIZE;
		return 0;

	/* Unsupported features. */
	case PIPE_CAP_BUFFER_SAMPLER_VIEW_RGBA_ONLY:
	case PIPE_CAP_TGSI_FS_COORD_ORIGIN_LOWER_LEFT:
	case PIPE_CAP_TGSI_CAN_COMPACT_CONSTANTS:
	case PIPE_CAP_USER_VERTEX_BUFFERS:
	case PIPE_CAP_FAKE_SW_MSAA:
	case PIPE_CAP_TEXTURE_GATHER_OFFSETS:
	case PIPE_CAP_VERTEXID_NOBASE:
	case PIPE_CAP_PRIMITIVE_RESTART_FOR_PATCHES:
	case PIPE_CAP_MAX_WINDOW_RECTANGLES:
	case PIPE_CAP_TGSI_FS_FBFETCH:
	case PIPE_CAP_TGSI_MUL_ZERO_WINS:
	case PIPE_CAP_UMA:
	case PIPE_CAP_POLYGON_MODE_FILL_RECTANGLE:
	case PIPE_CAP_POST_DEPTH_COVERAGE:
	case PIPE_CAP_TILE_RASTER_ORDER:
	case PIPE_CAP_MAX_COMBINED_SHADER_OUTPUT_RESOURCES:
	case PIPE_CAP_CONTEXT_PRIORITY_MASK:
		return 0;

	case PIPE_CAP_NATIVE_FENCE_FD:
		return sscreen->info.has_fence_to_handle;

	case PIPE_CAP_QUERY_BUFFER_OBJECT:
		return si_have_tgsi_compute(sscreen);

	case PIPE_CAP_DRAW_PARAMETERS:
	case PIPE_CAP_MULTI_DRAW_INDIRECT:
	case PIPE_CAP_MULTI_DRAW_INDIRECT_PARAMS:
		return sscreen->has_draw_indirect_multi;

	case PIPE_CAP_MAX_SHADER_PATCH_VARYINGS:
		return 30;

	case PIPE_CAP_TEXTURE_BORDER_COLOR_QUIRK:
		return sscreen->info.chip_class <= VI ?
			PIPE_QUIRK_TEXTURE_BORDER_COLOR_SWIZZLE_R600 : 0;

	/* Stream output. */
	case PIPE_CAP_MAX_STREAM_OUTPUT_SEPARATE_COMPONENTS:
	case PIPE_CAP_MAX_STREAM_OUTPUT_INTERLEAVED_COMPONENTS:
		return 32*4;

	/* Geometry shader output. */
	case PIPE_CAP_MAX_GEOMETRY_OUTPUT_VERTICES:
		return 1024;
	case PIPE_CAP_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS:
		return 4095;

	case PIPE_CAP_MAX_VERTEX_ATTRIB_STRIDE:
		return 2048;

	/* Texturing. */
	case PIPE_CAP_MAX_TEXTURE_2D_LEVELS:
	case PIPE_CAP_MAX_TEXTURE_CUBE_LEVELS:
		return 15; /* 16384 */
	case PIPE_CAP_MAX_TEXTURE_3D_LEVELS:
		/* textures support 8192, but layered rendering supports 2048 */
		return 12;
	case PIPE_CAP_MAX_TEXTURE_ARRAY_LAYERS:
		/* textures support 8192, but layered rendering supports 2048 */
		return 2048;

	/* Viewports and render targets. */
	case PIPE_CAP_MAX_VIEWPORTS:
		return SI_MAX_VIEWPORTS;
	case PIPE_CAP_VIEWPORT_SUBPIXEL_BITS:
	case PIPE_CAP_MAX_RENDER_TARGETS:
		return 8;

	case PIPE_CAP_MIN_TEXTURE_GATHER_OFFSET:
	case PIPE_CAP_MIN_TEXEL_OFFSET:
		return -32;

	case PIPE_CAP_MAX_TEXTURE_GATHER_OFFSET:
	case PIPE_CAP_MAX_TEXEL_OFFSET:
		return 31;

	case PIPE_CAP_ENDIANNESS:
		return PIPE_ENDIAN_LITTLE;

	case PIPE_CAP_VENDOR_ID:
		return ATI_VENDOR_ID;
	case PIPE_CAP_DEVICE_ID:
		return sscreen->info.pci_id;
	case PIPE_CAP_VIDEO_MEMORY:
		return sscreen->info.vram_size >> 20;
	case PIPE_CAP_PCI_GROUP:
		return sscreen->info.pci_domain;
	case PIPE_CAP_PCI_BUS:
		return sscreen->info.pci_bus;
	case PIPE_CAP_PCI_DEVICE:
		return sscreen->info.pci_dev;
	case PIPE_CAP_PCI_FUNCTION:
		return sscreen->info.pci_func;
	}
	return 0;
}

static float si_get_paramf(struct pipe_screen* pscreen, enum pipe_capf param)
{
	switch (param) {
	case PIPE_CAPF_MAX_LINE_WIDTH:
	case PIPE_CAPF_MAX_LINE_WIDTH_AA:
	case PIPE_CAPF_MAX_POINT_WIDTH:
	case PIPE_CAPF_MAX_POINT_WIDTH_AA:
		return 8192.0f;
	case PIPE_CAPF_MAX_TEXTURE_ANISOTROPY:
		return 16.0f;
	case PIPE_CAPF_MAX_TEXTURE_LOD_BIAS:
		return 16.0f;
	case PIPE_CAPF_GUARD_BAND_LEFT:
	case PIPE_CAPF_GUARD_BAND_TOP:
	case PIPE_CAPF_GUARD_BAND_RIGHT:
	case PIPE_CAPF_GUARD_BAND_BOTTOM:
		return 0.0f;
	}
	return 0.0f;
}

static int si_get_shader_param(struct pipe_screen* pscreen,
			       enum pipe_shader_type shader,
			       enum pipe_shader_cap param)
{
	struct si_screen *sscreen = (struct si_screen *)pscreen;

	switch(shader)
	{
	case PIPE_SHADER_FRAGMENT:
	case PIPE_SHADER_VERTEX:
	case PIPE_SHADER_GEOMETRY:
	case PIPE_SHADER_TESS_CTRL:
	case PIPE_SHADER_TESS_EVAL:
		break;
	case PIPE_SHADER_COMPUTE:
		switch (param) {
		case PIPE_SHADER_CAP_PREFERRED_IR:
			return PIPE_SHADER_IR_NATIVE;

		case PIPE_SHADER_CAP_SUPPORTED_IRS: {
			int ir = 1 << PIPE_SHADER_IR_NATIVE;

			if (si_have_tgsi_compute(sscreen))
				ir |= 1 << PIPE_SHADER_IR_TGSI;

			return ir;
		}

		case PIPE_SHADER_CAP_MAX_CONST_BUFFER_SIZE: {
			uint64_t max_const_buffer_size;
			pscreen->get_compute_param(pscreen, PIPE_SHADER_IR_TGSI,
				PIPE_COMPUTE_CAP_MAX_MEM_ALLOC_SIZE,
				&max_const_buffer_size);
			return MIN2(max_const_buffer_size, INT_MAX);
		}
		default:
			/* If compute shaders don't require a special value
			 * for this cap, we can return the same value we
			 * do for other shader types. */
			break;
		}
		break;
	default:
		return 0;
	}

	switch (param) {
	/* Shader limits. */
	case PIPE_SHADER_CAP_MAX_INSTRUCTIONS:
	case PIPE_SHADER_CAP_MAX_ALU_INSTRUCTIONS:
	case PIPE_SHADER_CAP_MAX_TEX_INSTRUCTIONS:
	case PIPE_SHADER_CAP_MAX_TEX_INDIRECTIONS:
	case PIPE_SHADER_CAP_MAX_CONTROL_FLOW_DEPTH:
		return 16384;
	case PIPE_SHADER_CAP_MAX_INPUTS:
		return shader == PIPE_SHADER_VERTEX ? SI_MAX_ATTRIBS : 32;
	case PIPE_SHADER_CAP_MAX_OUTPUTS:
		return shader == PIPE_SHADER_FRAGMENT ? 8 : 32;
	case PIPE_SHADER_CAP_MAX_TEMPS:
		return 256; /* Max native temporaries. */
	case PIPE_SHADER_CAP_MAX_CONST_BUFFER_SIZE:
		return 4096 * sizeof(float[4]); /* actually only memory limits this */
	case PIPE_SHADER_CAP_MAX_CONST_BUFFERS:
		return SI_NUM_CONST_BUFFERS;
	case PIPE_SHADER_CAP_MAX_TEXTURE_SAMPLERS:
	case PIPE_SHADER_CAP_MAX_SAMPLER_VIEWS:
		return SI_NUM_SAMPLERS;
	case PIPE_SHADER_CAP_MAX_SHADER_BUFFERS:
		return SI_NUM_SHADER_BUFFERS;
	case PIPE_SHADER_CAP_MAX_SHADER_IMAGES:
		return SI_NUM_IMAGES;
	case PIPE_SHADER_CAP_MAX_UNROLL_ITERATIONS_HINT:
		return 32;
	case PIPE_SHADER_CAP_PREFERRED_IR:
		if (sscreen->debug_flags & DBG(NIR))
			return PIPE_SHADER_IR_NIR;
		return PIPE_SHADER_IR_TGSI;
	case PIPE_SHADER_CAP_LOWER_IF_THRESHOLD:
		return 4;

	/* Supported boolean features. */
	case PIPE_SHADER_CAP_TGSI_CONT_SUPPORTED:
	case PIPE_SHADER_CAP_TGSI_SQRT_SUPPORTED:
	case PIPE_SHADER_CAP_INDIRECT_TEMP_ADDR:
	case PIPE_SHADER_CAP_INDIRECT_CONST_ADDR:
	case PIPE_SHADER_CAP_INTEGERS:
	case PIPE_SHADER_CAP_INT64_ATOMICS:
	case PIPE_SHADER_CAP_FP16:
	case PIPE_SHADER_CAP_TGSI_FMA_SUPPORTED:
	case PIPE_SHADER_CAP_TGSI_ANY_INOUT_DECL_RANGE:
	case PIPE_SHADER_CAP_TGSI_SKIP_MERGE_REGISTERS:
	case PIPE_SHADER_CAP_TGSI_DROUND_SUPPORTED:
	case PIPE_SHADER_CAP_TGSI_LDEXP_SUPPORTED:
	case PIPE_SHADER_CAP_TGSI_DFRACEXP_DLDEXP_SUPPORTED:
		return 1;

	case PIPE_SHADER_CAP_INDIRECT_INPUT_ADDR:
		/* TODO: Indirect indexing of GS inputs is unimplemented. */
		if (shader == PIPE_SHADER_GEOMETRY)
			return 0;

		if (shader == PIPE_SHADER_VERTEX &&
		    !sscreen->llvm_has_working_vgpr_indexing)
			return 0;

		/* TCS and TES load inputs directly from LDS or offchip
		 * memory, so indirect indexing is always supported.
		 * PS has to support indirect indexing, because we can't
		 * lower that to TEMPs for INTERP instructions.
		 */
		return 1;

	case PIPE_SHADER_CAP_INDIRECT_OUTPUT_ADDR:
		return sscreen->llvm_has_working_vgpr_indexing ||
		       /* TCS stores outputs directly to memory. */
		       shader == PIPE_SHADER_TESS_CTRL;

	/* Unsupported boolean features. */
	case PIPE_SHADER_CAP_SUBROUTINES:
	case PIPE_SHADER_CAP_SUPPORTED_IRS:
	case PIPE_SHADER_CAP_MAX_HW_ATOMIC_COUNTERS:
	case PIPE_SHADER_CAP_MAX_HW_ATOMIC_COUNTER_BUFFERS:
		return 0;
	}
	return 0;
}

static const struct nir_shader_compiler_options nir_options = {
	.vertex_id_zero_based = true,
	.lower_scmp = true,
	.lower_flrp32 = true,
	.lower_flrp64 = true,
	.lower_fsat = true,
	.lower_fdiv = true,
	.lower_sub = true,
	.lower_ffma = true,
	.lower_pack_snorm_2x16 = true,
	.lower_pack_snorm_4x8 = true,
	.lower_pack_unorm_2x16 = true,
	.lower_pack_unorm_4x8 = true,
	.lower_unpack_snorm_2x16 = true,
	.lower_unpack_snorm_4x8 = true,
	.lower_unpack_unorm_2x16 = true,
	.lower_unpack_unorm_4x8 = true,
	.lower_extract_byte = true,
	.lower_extract_word = true,
	.max_unroll_iterations = 32,
	.native_integers = true,
};

static const void *
si_get_compiler_options(struct pipe_screen *screen,
			enum pipe_shader_ir ir,
			enum pipe_shader_type shader)
{
	assert(ir == PIPE_SHADER_IR_NIR);
	return &nir_options;
}

static void si_get_driver_uuid(struct pipe_screen *pscreen, char *uuid)
{
	ac_compute_driver_uuid(uuid, PIPE_UUID_SIZE);
}

static void si_get_device_uuid(struct pipe_screen *pscreen, char *uuid)
{
	struct si_screen *sscreen = (struct si_screen *)pscreen;

	ac_compute_device_uuid(&sscreen->info, uuid, PIPE_UUID_SIZE);
}

static const char* si_get_name(struct pipe_screen *pscreen)
{
	struct si_screen *sscreen = (struct si_screen*)pscreen;

	return sscreen->renderer_string;
}

static int si_get_video_param_no_decode(struct pipe_screen *screen,
					enum pipe_video_profile profile,
					enum pipe_video_entrypoint entrypoint,
					enum pipe_video_cap param)
{
	switch (param) {
	case PIPE_VIDEO_CAP_SUPPORTED:
		return vl_profile_supported(screen, profile, entrypoint);
	case PIPE_VIDEO_CAP_NPOT_TEXTURES:
		return 1;
	case PIPE_VIDEO_CAP_MAX_WIDTH:
	case PIPE_VIDEO_CAP_MAX_HEIGHT:
		return vl_video_buffer_max_size(screen);
	case PIPE_VIDEO_CAP_PREFERED_FORMAT:
		return PIPE_FORMAT_NV12;
	case PIPE_VIDEO_CAP_PREFERS_INTERLACED:
		return false;
	case PIPE_VIDEO_CAP_SUPPORTS_INTERLACED:
		return false;
	case PIPE_VIDEO_CAP_SUPPORTS_PROGRESSIVE:
		return true;
	case PIPE_VIDEO_CAP_MAX_LEVEL:
		return vl_level_supported(screen, profile);
	default:
		return 0;
	}
}

static int si_get_video_param(struct pipe_screen *screen,
			      enum pipe_video_profile profile,
			      enum pipe_video_entrypoint entrypoint,
			      enum pipe_video_cap param)
{
	struct si_screen *sscreen = (struct si_screen *)screen;
	enum pipe_video_format codec = u_reduce_video_profile(profile);

	if (entrypoint == PIPE_VIDEO_ENTRYPOINT_ENCODE) {
		switch (param) {
		case PIPE_VIDEO_CAP_SUPPORTED:
			return codec == PIPE_VIDEO_FORMAT_MPEG4_AVC &&
				(si_vce_is_fw_version_supported(sscreen) ||
				sscreen->info.family == CHIP_RAVEN);
		case PIPE_VIDEO_CAP_NPOT_TEXTURES:
			return 1;
		case PIPE_VIDEO_CAP_MAX_WIDTH:
			return (sscreen->info.family < CHIP_TONGA) ? 2048 : 4096;
		case PIPE_VIDEO_CAP_MAX_HEIGHT:
			return (sscreen->info.family < CHIP_TONGA) ? 1152 : 2304;
		case PIPE_VIDEO_CAP_PREFERED_FORMAT:
			return PIPE_FORMAT_NV12;
		case PIPE_VIDEO_CAP_PREFERS_INTERLACED:
			return false;
		case PIPE_VIDEO_CAP_SUPPORTS_INTERLACED:
			return false;
		case PIPE_VIDEO_CAP_SUPPORTS_PROGRESSIVE:
			return true;
		case PIPE_VIDEO_CAP_STACKED_FRAMES:
			return (sscreen->info.family < CHIP_TONGA) ? 1 : 2;
		default:
			return 0;
		}
	}

	switch (param) {
	case PIPE_VIDEO_CAP_SUPPORTED:
		switch (codec) {
		case PIPE_VIDEO_FORMAT_MPEG12:
			return profile != PIPE_VIDEO_PROFILE_MPEG1;
		case PIPE_VIDEO_FORMAT_MPEG4:
			return 1;
		case PIPE_VIDEO_FORMAT_MPEG4_AVC:
			if ((sscreen->info.family == CHIP_POLARIS10 ||
			     sscreen->info.family == CHIP_POLARIS11) &&
			    sscreen->info.uvd_fw_version < UVD_FW_1_66_16 ) {
				RVID_ERR("POLARIS10/11 firmware version need to be updated.\n");
				return false;
			}
			return true;
		case PIPE_VIDEO_FORMAT_VC1:
			return true;
		case PIPE_VIDEO_FORMAT_HEVC:
			/* Carrizo only supports HEVC Main */
			if (sscreen->info.family >= CHIP_STONEY)
				return (profile == PIPE_VIDEO_PROFILE_HEVC_MAIN ||
					profile == PIPE_VIDEO_PROFILE_HEVC_MAIN_10);
			else if (sscreen->info.family >= CHIP_CARRIZO)
				return profile == PIPE_VIDEO_PROFILE_HEVC_MAIN;
			return false;
		case PIPE_VIDEO_FORMAT_JPEG:
			if (sscreen->info.family < CHIP_CARRIZO || sscreen->info.family >= CHIP_VEGA10)
				return false;
			if (!(sscreen->info.drm_major == 3 && sscreen->info.drm_minor >= 19)) {
				RVID_ERR("No MJPEG support for the kernel version\n");
				return false;
			}
			return true;
		default:
			return false;
		}
	case PIPE_VIDEO_CAP_NPOT_TEXTURES:
		return 1;
	case PIPE_VIDEO_CAP_MAX_WIDTH:
		return (sscreen->info.family < CHIP_TONGA) ? 2048 : 4096;
	case PIPE_VIDEO_CAP_MAX_HEIGHT:
		return (sscreen->info.family < CHIP_TONGA) ? 1152 : 4096;
	case PIPE_VIDEO_CAP_PREFERED_FORMAT:
		if (profile == PIPE_VIDEO_PROFILE_HEVC_MAIN_10)
			return PIPE_FORMAT_P016;
		else
			return PIPE_FORMAT_NV12;

	case PIPE_VIDEO_CAP_PREFERS_INTERLACED:
	case PIPE_VIDEO_CAP_SUPPORTS_INTERLACED: {
		enum pipe_video_format format = u_reduce_video_profile(profile);

		if (format == PIPE_VIDEO_FORMAT_HEVC)
			return false; //The firmware doesn't support interlaced HEVC.
		else if (format == PIPE_VIDEO_FORMAT_JPEG)
			return false;
		return true;
	}
	case PIPE_VIDEO_CAP_SUPPORTS_PROGRESSIVE:
		return true;
	case PIPE_VIDEO_CAP_MAX_LEVEL:
		switch (profile) {
		case PIPE_VIDEO_PROFILE_MPEG1:
			return 0;
		case PIPE_VIDEO_PROFILE_MPEG2_SIMPLE:
		case PIPE_VIDEO_PROFILE_MPEG2_MAIN:
			return 3;
		case PIPE_VIDEO_PROFILE_MPEG4_SIMPLE:
			return 3;
		case PIPE_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE:
			return 5;
		case PIPE_VIDEO_PROFILE_VC1_SIMPLE:
			return 1;
		case PIPE_VIDEO_PROFILE_VC1_MAIN:
			return 2;
		case PIPE_VIDEO_PROFILE_VC1_ADVANCED:
			return 4;
		case PIPE_VIDEO_PROFILE_MPEG4_AVC_BASELINE:
		case PIPE_VIDEO_PROFILE_MPEG4_AVC_MAIN:
		case PIPE_VIDEO_PROFILE_MPEG4_AVC_HIGH:
			return (sscreen->info.family < CHIP_TONGA) ? 41 : 52;
		case PIPE_VIDEO_PROFILE_HEVC_MAIN:
		case PIPE_VIDEO_PROFILE_HEVC_MAIN_10:
			return 186;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

static boolean si_vid_is_format_supported(struct pipe_screen *screen,
					  enum pipe_format format,
					  enum pipe_video_profile profile,
					  enum pipe_video_entrypoint entrypoint)
{
	/* HEVC 10 bit decoding should use P016 instead of NV12 if possible */
	if (profile == PIPE_VIDEO_PROFILE_HEVC_MAIN_10)
		return (format == PIPE_FORMAT_NV12) ||
			(format == PIPE_FORMAT_P016);

	/* we can only handle this one with UVD */
	if (profile != PIPE_VIDEO_PROFILE_UNKNOWN)
		return format == PIPE_FORMAT_NV12;

	return vl_video_buffer_is_format_supported(screen, format, profile, entrypoint);
}

static unsigned get_max_threads_per_block(struct si_screen *screen,
					  enum pipe_shader_ir ir_type)
{
	if (ir_type != PIPE_SHADER_IR_TGSI)
		return 256;

	/* Only 16 waves per thread-group on gfx9. */
	if (screen->info.chip_class >= GFX9)
		return 1024;

	/* Up to 40 waves per thread-group on GCN < gfx9. Expose a nice
	 * round number.
	 */
	return 2048;
}

static int si_get_compute_param(struct pipe_screen *screen,
				enum pipe_shader_ir ir_type,
				enum pipe_compute_cap param,
				void *ret)
{
	struct si_screen *sscreen = (struct si_screen *)screen;

	//TODO: select these params by asic
	switch (param) {
	case PIPE_COMPUTE_CAP_IR_TARGET: {
		const char *gpu;
		const char *triple;

		if (HAVE_LLVM < 0x0400)
			triple = "amdgcn--";
		else
			triple = "amdgcn-mesa-mesa3d";

		gpu = ac_get_llvm_processor_name(sscreen->info.family);
		if (ret) {
			sprintf(ret, "%s-%s", gpu, triple);
		}
		/* +2 for dash and terminating NIL byte */
		return (strlen(triple) + strlen(gpu) + 2) * sizeof(char);
	}
	case PIPE_COMPUTE_CAP_GRID_DIMENSION:
		if (ret) {
			uint64_t *grid_dimension = ret;
			grid_dimension[0] = 3;
		}
		return 1 * sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_GRID_SIZE:
		if (ret) {
			uint64_t *grid_size = ret;
			grid_size[0] = 65535;
			grid_size[1] = 65535;
			grid_size[2] = 65535;
		}
		return 3 * sizeof(uint64_t) ;

	case PIPE_COMPUTE_CAP_MAX_BLOCK_SIZE:
		if (ret) {
			uint64_t *block_size = ret;
			unsigned threads_per_block = get_max_threads_per_block(sscreen, ir_type);
			block_size[0] = threads_per_block;
			block_size[1] = threads_per_block;
			block_size[2] = threads_per_block;
		}
		return 3 * sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_THREADS_PER_BLOCK:
		if (ret) {
			uint64_t *max_threads_per_block = ret;
			*max_threads_per_block = get_max_threads_per_block(sscreen, ir_type);
		}
		return sizeof(uint64_t);
	case PIPE_COMPUTE_CAP_ADDRESS_BITS:
		if (ret) {
			uint32_t *address_bits = ret;
			address_bits[0] = 64;
		}
		return 1 * sizeof(uint32_t);

	case PIPE_COMPUTE_CAP_MAX_GLOBAL_SIZE:
		if (ret) {
			uint64_t *max_global_size = ret;
			uint64_t max_mem_alloc_size;

			si_get_compute_param(screen, ir_type,
				PIPE_COMPUTE_CAP_MAX_MEM_ALLOC_SIZE,
				&max_mem_alloc_size);

			/* In OpenCL, the MAX_MEM_ALLOC_SIZE must be at least
			 * 1/4 of the MAX_GLOBAL_SIZE.  Since the
			 * MAX_MEM_ALLOC_SIZE is fixed for older kernels,
			 * make sure we never report more than
			 * 4 * MAX_MEM_ALLOC_SIZE.
			 */
			*max_global_size = MIN2(4 * max_mem_alloc_size,
						MAX2(sscreen->info.gart_size,
						     sscreen->info.vram_size));
		}
		return sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_LOCAL_SIZE:
		if (ret) {
			uint64_t *max_local_size = ret;
			/* Value reported by the closed source driver. */
			*max_local_size = 32768;
		}
		return sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_INPUT_SIZE:
		if (ret) {
			uint64_t *max_input_size = ret;
			/* Value reported by the closed source driver. */
			*max_input_size = 1024;
		}
		return sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_MEM_ALLOC_SIZE:
		if (ret) {
			uint64_t *max_mem_alloc_size = ret;

			*max_mem_alloc_size = sscreen->info.max_alloc_size;
		}
		return sizeof(uint64_t);

	case PIPE_COMPUTE_CAP_MAX_CLOCK_FREQUENCY:
		if (ret) {
			uint32_t *max_clock_frequency = ret;
			*max_clock_frequency = sscreen->info.max_shader_clock;
		}
		return sizeof(uint32_t);

	case PIPE_COMPUTE_CAP_MAX_COMPUTE_UNITS:
		if (ret) {
			uint32_t *max_compute_units = ret;
			*max_compute_units = sscreen->info.num_good_compute_units;
		}
		return sizeof(uint32_t);

	case PIPE_COMPUTE_CAP_IMAGES_SUPPORTED:
		if (ret) {
			uint32_t *images_supported = ret;
			*images_supported = 0;
		}
		return sizeof(uint32_t);
	case PIPE_COMPUTE_CAP_MAX_PRIVATE_SIZE:
		break; /* unused */
	case PIPE_COMPUTE_CAP_SUBGROUP_SIZE:
		if (ret) {
			uint32_t *subgroup_size = ret;
			*subgroup_size = 64;
		}
		return sizeof(uint32_t);
	case PIPE_COMPUTE_CAP_MAX_VARIABLE_THREADS_PER_BLOCK:
		if (ret) {
			uint64_t *max_variable_threads_per_block = ret;
			if (ir_type == PIPE_SHADER_IR_TGSI)
				*max_variable_threads_per_block = SI_MAX_VARIABLE_THREADS_PER_BLOCK;
			else
				*max_variable_threads_per_block = 0;
		}
		return sizeof(uint64_t);
	}

        fprintf(stderr, "unknown PIPE_COMPUTE_CAP %d\n", param);
        return 0;
}

static uint64_t si_get_timestamp(struct pipe_screen *screen)
{
	struct si_screen *sscreen = (struct si_screen*)screen;

	return 1000000 * sscreen->ws->query_value(sscreen->ws, RADEON_TIMESTAMP) /
			sscreen->info.clock_crystal_freq;
}

static void si_query_memory_info(struct pipe_screen *screen,
				 struct pipe_memory_info *info)
{
	struct si_screen *sscreen = (struct si_screen*)screen;
	struct radeon_winsys *ws = sscreen->ws;
	unsigned vram_usage, gtt_usage;

	info->total_device_memory = sscreen->info.vram_size / 1024;
	info->total_staging_memory = sscreen->info.gart_size / 1024;

	/* The real TTM memory usage is somewhat random, because:
	 *
	 * 1) TTM delays freeing memory, because it can only free it after
	 *    fences expire.
	 *
	 * 2) The memory usage can be really low if big VRAM evictions are
	 *    taking place, but the real usage is well above the size of VRAM.
	 *
	 * Instead, return statistics of this process.
	 */
	vram_usage = ws->query_value(ws, RADEON_REQUESTED_VRAM_MEMORY) / 1024;
	gtt_usage =  ws->query_value(ws, RADEON_REQUESTED_GTT_MEMORY) / 1024;

	info->avail_device_memory =
		vram_usage <= info->total_device_memory ?
				info->total_device_memory - vram_usage : 0;
	info->avail_staging_memory =
		gtt_usage <= info->total_staging_memory ?
				info->total_staging_memory - gtt_usage : 0;

	info->device_memory_evicted =
		ws->query_value(ws, RADEON_NUM_BYTES_MOVED) / 1024;

	if (sscreen->info.drm_major == 3 && sscreen->info.drm_minor >= 4)
		info->nr_device_memory_evictions =
			ws->query_value(ws, RADEON_NUM_EVICTIONS);
	else
		/* Just return the number of evicted 64KB pages. */
		info->nr_device_memory_evictions = info->device_memory_evicted / 64;
}

static struct disk_cache *si_get_disk_shader_cache(struct pipe_screen *pscreen)
{
	struct si_screen *sscreen = (struct si_screen*)pscreen;

	return sscreen->disk_shader_cache;
}

static void si_init_renderer_string(struct si_screen *sscreen)
{
	struct radeon_winsys *ws = sscreen->ws;
	char family_name[32] = {}, llvm_string[32] = {}, kernel_version[128] = {};
	struct utsname uname_data;

	const char *chip_name = si_get_marketing_name(ws);

	if (chip_name)
		snprintf(family_name, sizeof(family_name), "%s / ",
			 si_get_family_name(sscreen) + 4);
	else
		chip_name = si_get_family_name(sscreen);

	if (uname(&uname_data) == 0)
		snprintf(kernel_version, sizeof(kernel_version),
			 " / %s", uname_data.release);

	if (HAVE_LLVM > 0) {
		snprintf(llvm_string, sizeof(llvm_string),
			 ", LLVM %i.%i.%i", (HAVE_LLVM >> 8) & 0xff,
			 HAVE_LLVM & 0xff, MESA_LLVM_VERSION_PATCH);
	}

	snprintf(sscreen->renderer_string, sizeof(sscreen->renderer_string),
		 "%s (%sDRM %i.%i.%i%s%s)",
		 chip_name, family_name, sscreen->info.drm_major,
		 sscreen->info.drm_minor, sscreen->info.drm_patchlevel,
		 kernel_version, llvm_string);
}

void si_init_screen_get_functions(struct si_screen *sscreen)
{
	sscreen->b.get_name = si_get_name;
	sscreen->b.get_vendor = si_get_vendor;
	sscreen->b.get_device_vendor = si_get_device_vendor;
	sscreen->b.get_param = si_get_param;
	sscreen->b.get_paramf = si_get_paramf;
	sscreen->b.get_compute_param = si_get_compute_param;
	sscreen->b.get_timestamp = si_get_timestamp;
	sscreen->b.get_shader_param = si_get_shader_param;
	sscreen->b.get_compiler_options = si_get_compiler_options;
	sscreen->b.get_device_uuid = si_get_device_uuid;
	sscreen->b.get_driver_uuid = si_get_driver_uuid;
	sscreen->b.query_memory_info = si_query_memory_info;
	sscreen->b.get_disk_shader_cache = si_get_disk_shader_cache;

	if (sscreen->info.has_hw_decode) {
		sscreen->b.get_video_param = si_get_video_param;
		sscreen->b.is_video_format_supported = si_vid_is_format_supported;
	} else {
		sscreen->b.get_video_param = si_get_video_param_no_decode;
		sscreen->b.is_video_format_supported = vl_video_buffer_is_format_supported;
	}

	si_init_renderer_string(sscreen);
}
