package mtkDebugFW

import (
	"android/soong/android"
	"android/soong/cc"
)

func mtkDebugFWDefaults(ctx android.LoadHookContext) {
	type props struct {
		Target struct {
			Android_arm struct {
				Cflags []string
			}
		}
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.Bool("MSSI_MTK_USER_SPACE_DEBUG_FW") && (vars.String("TARGET_BUILD_VARIANT") == "eng") {
		p.Target.Android_arm.Cflags = append(p.Target.Android_arm.Cflags, "-marm")
		p.Target.Android_arm.Cflags = append(p.Target.Android_arm.Cflags, "-fno-omit-frame-pointer")
	}
	ctx.AppendProperties(p)
}

func init() {
	android.RegisterModuleType("mtk_debug_fw_defaults", mtkDebugFWDefaultsFactory)
}

func mtkDebugFWDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkDebugFWDefaults)
	return module
}
