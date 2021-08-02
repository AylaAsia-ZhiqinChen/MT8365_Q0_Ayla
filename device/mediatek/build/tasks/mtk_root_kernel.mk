
ifeq (yes, $(strip $(MTK_BUILD_ROOT)))
ifneq (,$(wildcard vendor/mediatek/internal/kernel_root))

post_kernel_root_src := vendor/mediatek/internal/kernel_root/do_mounts_post.c
kernel_root_src := $(KERNEL_ROOT_OUT)/init/do_mounts_post.c

$(BUILT_KERNEL_ROOT_TARGET): $(kernel_root_src)
$(kernel_root_src): $(post_kernel_root_src)
	$(hide) mkdir -p $(KERNEL_ROOT_OUT)/init
	$(hide) cp -f $(post_kernel_root_src) $(kernel_root_src)

endif #vendor/mediatek/internal/kernel_root
endif # MTK_BUILD_ROOT
