package wifidisplay

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkwifidisplayDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Srcs   []string
		Static_libs  []string
		Shared_libs  []string
		
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	p.Srcs = append(p.Srcs, "MediaReceiver.cpp","TimeSyncer.cpp",
      "rtp/RTPAssembler.cpp", "rtp/RTPReceiver.cpp",
      "sink/DirectRenderer.cpp", "sink/WifiDisplaySink.cpp", "hdcprx/mux_demux_ex.cpp",)
    p.Static_libs = append(p.Static_libs, "libstagefright_mpeg2support_mtk")
    p.Shared_libs = append(p.Shared_libs, "libhidlallocatorutils","libhidlbase","android.hardware.cas@1.0","android.hardware.cas.native@1.0","libcrypto","libmediandk","android.hidl.token@1.0-utils","android.hidl.allocator@1.0","libhidlmemory","liblog")

	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DWFD_HDCP_RX_SUPPORT")
		p.Cflags = append(p.Cflags, "-DMTK_WFD_HDCP_RX_SUPPORT")
		p.Cflags = append(p.Cflags, "-DHDCP2_RX_VER")
	}
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
	  p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_SUPPORT")
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_UIBC_SUPPORT")
		p.Cflags = append(p.Cflags, "-DUIBC_SOURCE_SUPPORT")
	}
  p.Cflags = append(p.Cflags, "-DMTK_AOSP_ENHANCEMENT")
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_wifidisplay_defaults", mtkwifidisplayDefaultsFactory)
}

func mtkwifidisplayDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkwifidisplayDefaults)
	return module
}
