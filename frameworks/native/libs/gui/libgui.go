package libgui

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkLibguiDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")

	if vars.Bool("MSSI_MTK_GRAPHIC_LOW_LATENCY") {
		p.Cflags = append(p.Cflags, "-DMSSI_MTK_GRAPHIC_LOW_LATENCY")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libgui_defaults", mtkLibguiDefaultsFactory)
}

func mtkLibguiDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibguiDefaults)
	return module
}
