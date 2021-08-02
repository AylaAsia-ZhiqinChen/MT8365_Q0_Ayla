package mediaplayerservice

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkMediaplayerserviceDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags       []string
		Include_dirs []string
		Shared_libs  []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_SUPPORT")
	}
	if vars.Bool("MSSI_MTK_WFD_SINK_UIBC_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_SINK_UIBC_SUPPORT")
	}

	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libmediaplayerservice_defaults", mtkMediaplayerserviceDefaultsFactory)
}

func mtkMediaplayerserviceDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkMediaplayerserviceDefaults)
	return module
}
