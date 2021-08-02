YUNTIAN_TARGET_PROJECT := $(subst full_,,$(TARGET_PRODUCT))
##(0) buildinfo  Begin
$(shell test -d out || mkdir out)
$(echo shell printenv)
$(shell printenv | grep -E 'TARGET_BUILD_VARIANT=|TARGET_PRODUCT=' > out/buildinfo.txt)
##(0) buildinfo  End


