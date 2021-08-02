package mtkJpegEnhanceArt
import (
    "android/soong/android"
    "android/soong/cc"
)

func mtkJpegEnhanceArtDefaults(ctx android.LoadHookContext) {
    type props struct {
        Target struct {
            Android struct {
                Cflags []string
                Include_dirs []string
                Srcs []string
            }
        }
    }
    p := &props{}

    var includeDirs []string
    var srcs []string

    p.Target.Android.Cflags = append(p.Target.Android.Cflags, "-DMTK_IMAGE_ENABLE_PQ_FOR_JPEG")

    includeDirs = append(includeDirs, "external/skia/include/mtk")

    srcs = append(srcs, "android/graphics/mtk/BitmapRegionDecoder.cpp")

    p.Target.Android.Include_dirs = includeDirs
    p.Target.Android.Srcs = srcs

    ctx.AppendProperties(p)
}

func init() {
    android.RegisterModuleType("mtk_jpeg_enhance_art_defaults", mtkJpegEnhanceArtDefaultsFactory)
}

func mtkJpegEnhanceArtDefaultsFactory() (android.Module) {
    module := cc.DefaultsFactory()
    android.AddLoadHook(module, mtkJpegEnhanceArtDefaults)
    return module
}
