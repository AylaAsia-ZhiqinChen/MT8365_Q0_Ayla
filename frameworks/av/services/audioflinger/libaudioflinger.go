package audioflinger

import (
	"android/soong/android"
	"android/soong/cc"
)
func mtkLibAudioFlingerDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Include_dirs []string
		Shared_libs []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MTK_AUDIO") {
		p.Cflags = append(p.Cflags, "-DMTK_AUDIO", "-DMTK_AUDIO_DEBUG", "-DMTK_LOW_LATENCY", "-DMTK_LOW_POWER", "-DMTK_AUDIO_FIX_DEFAULT_DEFECT")
	}

	if (vars.String("TARGET_BUILD_VARIANT") == "eng") || (vars.String("TARGET_BUILD_VARIANT") == "userdebug") {
		p.Cflags = append(p.Cflags, "-DMTK_LATENCY_DETECT_PULSE")
		p.Shared_libs = append(p.Shared_libs, "libmtkaudio_utils")
		p.Include_dirs = append(p.Include_dirs, "vendor/mediatek/proprietary/external/audio_utils/common_utils/AudioDetectPulse")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_libaudioflinger_defaults", mtkLibAudioFlingerDefaultsFactory)
}

func mtkLibAudioFlingerDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkLibAudioFlingerDefaults)
	return module
}