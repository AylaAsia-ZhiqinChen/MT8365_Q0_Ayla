package libmedia

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkLibmediaDefaults(ctx android.LoadHookContext) {

	type props struct {
		Cflags       []string
		Srcs   []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_SUPPORT")
	}
	if vars.Bool("MSSI_MTK_WFD_SINK_UIBC_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_UIBC_SUPPORT")
	}
	p.Cflags = append(p.Cflags, "-DMTK_AOSP_ENHANCEMENT")

	ctx.AppendProperties(p)

}

func init() {
	android.RegisterModuleType("mtk_libmedia_defaults", mtkLibmediaDefaultsFactory)
}

func mtkLibmediaDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibmediaDefaults)
	return module
}
