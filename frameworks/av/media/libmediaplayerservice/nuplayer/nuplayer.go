package nuplayer

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkNuplayerDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags       []string
		Include_dirs []string
		Shared_libs  []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_AUDIO_APE_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMSSI_MTK_AUDIO_APE_SUPPORT")
	}
	if vars.Bool("MSSI_MTK_WFD_SINK_SUPPORT") {
		p.Cflags = append(p.Cflags, "-DMTK_WFD_HDCP_RX_SUPPORT")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libstagefright_nuplayer_defaults", mtkNuplayerDefaultsFactory)
}

func mtkNuplayerDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkNuplayerDefaults)
	return module
}
