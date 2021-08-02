#define DEBUG_LOG_TAG "PLAT"

#include "gralloc_mtk_defs.h"

#include <hardware/hwcomposer.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "utils/debug.h"
#include "utils/tools.h"

#include "hwc2.h"
#include "platform.h"

extern unsigned int mapDpOrientation(const uint32_t transform);

// ---------------------------------------------------------------------------

ANDROID_SINGLETON_STATIC_INSTANCE(Platform);

Platform::Platform()
{
    m_config.platform = PLATFORM_MT8167;

    m_config.compose_level = COMPOSE_ENABLE_ALL;

    m_config.mirror_state = MIRROR_DISABLED;

    // m_config.overlay_cap = (OVL_CAP_DIM | OVL_CAP_DIM_HW | OVL_CAP_P_FENCE);

    // m_config.ovl_overlap_limit = 4;

    m_config.use_async_bliter_ultra = false;

    m_config.enable_smart_layer = false;

    m_config.enable_rgba_rotate = false;

    m_config.enable_rgbx_scaling = false;

    m_config.av_grouping = false;

    m_config.only_wfd_by_dispdev = true;

    m_config.is_skip_validate = true;

    m_config.support_color_transform = true;
}

void Platform::initOverlay()
{
}

size_t Platform::getLimitedExternalDisplaySize()
{
    // fhd resolution
    return 1920 * 1080;
}

bool Platform::isUILayerValid(const sp<HWCLayer>& layer, int32_t* line)
{
    return PlatformCommon::isUILayerValid(layer, line);
}

bool Platform::isMMLayerValid(const sp<HWCLayer>& layer, int32_t* line)
{
    return PlatformCommon::isMMLayerValid(layer, line);
}
