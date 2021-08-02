package libaaudio

import (
	"android/soong/android"
	"android/soong/cc"
)
func mtklibaaudioDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Shared_libs []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MTK_AUDIO") {
		p.Cflags = append(p.Cflags, "-DMTK_AUDIO_DEBUG")
		p.Shared_libs = append(p.Shared_libs, "libaudioutilmtk")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libaaudio_defaults", mtklibaaudioDefaultsFactory)
}

func mtklibaaudioDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtklibaaudioDefaults)
	return module
}