package audioprocessing

import (
	"android/soong/android"
	"android/soong/cc"
)
func mtkLibAudioProcessingDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MTK_AUDIO") {
		p.Cflags = append(p.Cflags, "-DMTK_AUDIO", "-DMTK_AUDIO_DEBUG", "-DMTK_AUDIO_FIX_DEFAULT_DEFECT")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libaudioprocessing_defaults", mtkLibAudioProcessingDefaultsFactory)
}

func mtkLibAudioProcessingDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibAudioProcessingDefaults)
	return module
}