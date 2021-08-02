package wfdjni

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkWfdJniDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_SUPPORT")
	}
    if vars.Bool("MSSI_MTK_WFD_SINK_UIBC_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_UIBC_SUPPORT")
	}
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_HDCP_RX_SUPPORT")
	}
	p.Cflags = append(p.Cflags, "-DMTK_AOSP_ENHANCEMENT")
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_wfd_jni_defaults", mtkWfdJniDefaultsFactory)
}

func mtkWfdJniDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkWfdJniDefaults)
	return module
}