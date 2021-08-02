package mpeg2atsparser

import (
	"android/soong/android"
	"android/soong/cc"
)

func mpeg2atsparserDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DWFD_HDCP_RX_SUPPORT")
		p.Cflags = append(p.Cflags, "-DMTK_WFD_HDCP_RX_SUPPORT")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_mpeg2atsparser_defaults", mpeg2atsparserDefaultsFactory)
}

func mpeg2atsparserDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mpeg2atsparserDefaults)
	return module
}
