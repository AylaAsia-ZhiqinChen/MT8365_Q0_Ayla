package audioclient

import (
	"android/soong/android"
	"android/soong/cc"
)
func mtkLibAudioclientDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Shared_libs []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MTK_AUDIO") {
		p.Cflags = append(p.Cflags, "-DMTK_AUDIO", "-DMTK_AUDIO_DEBUG", "-DMTK_AUDIO_FIX_DEFAULT_DEFECT", "-DMTK_AUDIO_GAIN")
		p.Shared_libs = append(p.Shared_libs, "libaudioutilmtk")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libaudioclient_defaults", mtkLibAudioclientDefaultsFactory)
}

func mtkLibAudioclientDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibAudioclientDefaults)
	return module
}