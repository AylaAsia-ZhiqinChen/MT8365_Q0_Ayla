package audiopolicyenginedefault

import (
	"android/soong/android"
	"android/soong/cc"
)
func mtkLibAudiopolicyenginedefaultDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Shared_libs []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MTK_AUDIO") {
		p.Cflags = append(p.Cflags, "-DMTK_AUDIO", "-DMTK_AUDIO_DEBUG", "-DMTK_AUDIO_FIX_DEFAULT_DEFECT","-DMTK_LOW_LATENCY","-DSYS_IMPL" )
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libaudiopolicyenginedefault_defaults", mtkLibAudiopolicyenginedefaultDefaultsFactory)
}

func mtkLibAudiopolicyenginedefaultDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibAudiopolicyenginedefaultDefaults)
	return module
}
