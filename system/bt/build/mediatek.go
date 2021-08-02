package mediatekBluetooth

import (
	"android/soong/android"
	"android/soong/cc"
	"strings"
	"fmt"
)

var bChipFlag bool

func MtkGlobalDefaults(ctx android.BaseContext) ([]string, []string) {
	var cflags []string
	var includeDirs []string
	var vars android.VendorConfig
	fmt.Println("BT_TUNNEL_SUPPORT:", ctx.AConfig().IsEnvTrue("BT_TUNNEL_SUPPORT"))
	if ctx.AConfig().IsEnvTrue("BT_TUNNEL_SUPPORT") {
		cflags = append(cflags, "-DBT_TUNNEL_SUPPORT=TRUE")
	} else {
		cflags = append(cflags, "-DBT_TUNNEL_SUPPORT=FALSE")
	}
	/************************************************
	* ** General Config
	* ***********************************************/
	//vars := ctx.Config().VendorConfig("mtkPlugin")
	cflags = append(cflags, "-DHAS_MDROID_BUILDCFG")
	includeDirs = append(includeDirs, "system/bt/mediatek/include")

	// MTK CE CHIP define
	chipName := ctx.AConfig().Getenv("WIFI_DONGLE")
	if (strings.HasPrefix(chipName, "MT76")) {
		fmt.Println("WIFI_DONGLE --- :", chipName)
		bChipFlag = true
		cflags = append(cflags, "-DMTK_BT_CHIP_76XX")
	} else {
		vars = ctx.Config().VendorConfig("mtkPlugin")
		chipName := vars.String("MTK_BT_CHIP")
		if (strings.HasPrefix(chipName, "MTK_MT76")) {
			fmt.Println("MTK_BT_CHIP --- :", chipName)
			cflags = append(cflags, "-DMTK_BT_CHIP_76XX")
		}
	}

	// MTK interop extension
	if bChipFlag {
		cflags = append(cflags, "-DMTK_INTEROP_EXTENSION=TRUE")
	} else {
		if vars.Bool("MSSI_MTK_BT_INTEROP_EXTENSION") {
			cflags = append(cflags, "-DMTK_INTEROP_EXTENSION=TRUE")
		}
	}

	// Custom AVRCP APP Settings feature
	cflags = append(cflags, "-DMTK_AVRCP_APP_SETTINGS=TRUE")

	// Enable BLE VND Feature for MTK BT
	cflags = append(cflags, "-DBLE_VND_INCLUDED=TRUE")

	// MTK adjust a2dp packet length of SBC as 2-DH5/3-DH5 for firmware
	cflags = append(cflags, "-DMTK_VND_A2DP_PKT_LEN=TRUE")

	// MTK modifies the AOSP for bug fixing
	cflags = append(cflags, "-DMTK_BT_PROPRIETARY_HANDLING=TRUE")

	// MTK support customized snoop log
	if bChipFlag {
		cflags = append(cflags, "-DMTK_STACK_CONFIG_LOG=TRUE")
	} else {
		if vars.Bool("MSSI_MTK_BT_HOST_SNOOP_LOG_SUPPORT") {
			cflags = append(cflags, "-DMTK_STACK_CONFIG_LOG=TRUE")
		}
	}

	// MTK a2dp hal layer PCM dump
	cflags = append(cflags, "-DMTK_A2DP_PCM_DUMP=TRUE")

	// Enable Android MTK MESH support
	cflags = append(cflags, "-DMTK_MESH_SUPPORT")
	cflags = append(cflags, "-DMTK_ANDROID_MESH=TRUE")
	cflags = append(cflags, "-DMTK_GATT_BEARER_SUPPORT")

	return cflags, includeDirs
}

func init() {
	android.RegisterModuleType("mtk_hcidebug_defaults", mtkHciDebugDefaultsFactory)
}

func mtkHciDebugDefaultsFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, mtkHciDebugDefaults)
	return module
}

func mtkHciDebugDefaults(ctx android.LoadHookContext) {
	type props struct {
		Cflags []string
		Include_dirs   []string
		Shared_libs []string
	}
	p := &props{}
	vars := ctx.Config().VendorConfig("mtkPlugin")
	if vars.String("TARGET_BUILD_VARIANT") != "user" {
		// MTK hci debug
		p.Cflags = append(p.Cflags, "-DMTK_HCI_DEBUG=1")
		if vars.Bool("HAVE_AEE_FEATURE") {
			p.Cflags = append(p.Cflags, "-DHAVE_AEE_FEATURE")
			p.Include_dirs = append(p.Include_dirs, "vendor/mediatek/proprietary/external/aee/binary/inc")
			p.Shared_libs = append(p.Shared_libs, "libaed")
		}
		p.Shared_libs = append(p.Shared_libs, "libutilscallstack")
	}
	ctx.AppendProperties(p)
}
