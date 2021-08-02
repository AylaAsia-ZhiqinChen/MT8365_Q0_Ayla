#!/bin/bash

#=========================================
# Example: MTK6739
# $1=out/target/product/k39tv1_64_bsp
# $2=device/mediatek/security/releasekey
# $3=k39tv1_64_bsp
# $4=out/host/linux-x86/lib64
# $5=.so
# $6=out/host/linux-x86/bin
# $7=out/host/linux-x86/framework
#=========================================

ROOTPATH="target_files-package"
mkdir -p $ROOTPATH
#build
mkdir -p  $ROOTPATH/build/target/product/security/
cp -a build/target/product/security/  $ROOTPATH/build/target/product/security/
mkdir -p $ROOTPATH/build/tools/releasetools/
cp -a build/tools/releasetools/  $ROOTPATH/build/tools/releasetools/

#out-bin
mkdir -p $ROOTPATH/out/host/linux-x86/bin/
cp -a $6/minigzip \
$6/mkbootfs \
$6/mkbootimg \
$6/fs_config \
$6/zipalign \
$6/bsdiff \
$6/imgdiff \
$6/mke2fs \
$6/mkuserimg_mke2fs \
$6/e2fsdroid \
$6/tune2fs \
$6/make_f2fs \
$6/sload_f2fs \
$6/simg2img \
$6/e2fsck \
$6/fec \
$6/care_map_generator \
$6/fc_sort \
$6/sefcontext_compile \
$6/blk_alloc_to_base_fs \
$6/brotli \
$ROOTPATH/out/host/linux-x86/bin/

#out-framwork
mkdir -p $ROOTPATH/out/host/linux-x86/framework/
cp -a $7/signapk.jar $ROOTPATH/out/host/linux-x86/framework/
#$7/dumpkey.jar \
#$7/BootSignature.jar \
#$7/VeritySigner.jar \

#out-lib/lib64
mkdir -p $ROOTPATH/out/host/linux-x86/lib64/
cp -a $4/libc++$5 \
$4/liblog$5 \
$4/libcutils$5 \
$4/libselinux$5 \
$4/libcrypto-host$5 \
$4/libext2fs-host$5 \
$4/libext2_blkid-host$5 \
$4/libext2_com_err-host$5 \
$4/libext2_e2p-host$5 \
$4/libext2_quota-host$5 \
$4/libext2_uuid-host$5 \
$4/libconscrypt_openjdk_jni$5 \
$4/libprotobuf-cpp-lite$5 \
$4/libz-host$5 \
$4/libsparse-host$5 \
$4/libbase$5 \
$4/libpcre2$5 \
$4/libbrotli$5 \
$4/liblp$5 \
$4/libext4_utils$5 \
$ROOTPATH/out/host/linux-x86/lib64/

#ab update 
if [ "$8"x = "yes"x ] || [ "$8"x = "true"x ] ; then
cp -a $6/brillo_update_payload \
$6/delta_generator \
$ROOTPATH/out/host/linux-x86/bin/

mkdir -p $ROOTPATH/out/host/linux-x86/bin/lib/shflags/
cp -a $6/lib/shflags/shflags $ROOTPATH/out/host/linux-x86/bin/lib/shflags/

cp -a $4/libbrillo$5 \
$4/libbrillo-stream$5 \
$4/libchrome$5 \
$4/libfec$5 \
$4/libcrypto_utils$5 \
$4/libevent-host$5 \
$4/libsquashfs_utils$5 \
$4/libssl-host$5 \
$ROOTPATH/out/host/linux-x86/lib64/
fi

#system
mkdir -p $ROOTPATH/system/extras/ext4_utils/
cp -ar system/extras/ext4_utils/ $ROOTPATH/system/extras/ext4_utils/
mkdir -p $ROOTPATH/system/extras/verity/
cp -a  system/extras/verity/build_verity_metadata.py $ROOTPATH/system/extras/verity/
#vendor
mkdir -p $ROOTPATH/vendor/mediatek/proprietary/custom/$3/security/
cp -ar vendor/mediatek/proprietary/custom/$3/security/  $ROOTPATH/vendor/mediatek/proprietary/custom/$3/security/
mkdir -p $ROOTPATH/vendor/mediatek/proprietary/scripts/releasetools/
cp -ar vendor/mediatek/proprietary/scripts/releasetools/  $ROOTPATH/vendor/mediatek/proprietary/scripts/releasetools/
#device key
keydir=$"`dirname $2`"
mkdir -p $ROOTPATH/$keydir
cp -a $keydir/*  $ROOTPATH/$keydir/
#system_image_info
cp -a $1/obj/PACKAGING/systemimage_intermediates/system_image_info.txt $ROOTPATH/
cp -a $1/obj/PACKAGING/vendor_intermediates/vendor_image_info.txt $ROOTPATH/
mkdir -p $ROOTPATH/out/target/product/$3/obj/ETC/file_contexts.bin_intermediates/
cp -a $1/obj/ETC/file_contexts.bin_intermediates/file_contexts.bin  $ROOTPATH/out/target/product/$3/obj/ETC/file_contexts.bin_intermediates/
#scatter
cp -a $1/ota_scatter.txt $ROOTPATH/scatter.txt
#target_files
echo `ls -lrt $1/obj/PACKAGING/target_files_intermediates/*target_files*.zip|tail -n 1|awk '{print $NF}'`
cp -a `ls -lrt $1/obj/PACKAGING/target_files_intermediates/*target_files*.zip|tail -n 1|awk '{print $NF}'`  $ROOTPATH/ota_target_files.zip

#build.prop
cp -a $1/system/build.prop $ROOTPATH/build.prop

#configure.xml
echo "">$ROOTPATH/configure.xml
echo "<root>">>$ROOTPATH/configure.xml

#buildnumber
var=$(grep  "ro.fota.version=" "$1/system/build.prop" )
buildnumber=${var##"ro.fota.version="}
echo "<buildnumber>$buildnumber</buildnumber>">>$ROOTPATH/configure.xml

#language
if [ -n "`grep "ro.fota.language=" $1/system/build.prop`" ] ; then
    var=$(grep  "ro.fota.language=" "$1/system/build.prop" )
    echo "<language>${var##"ro.fota.language="}</language>">>$ROOTPATH/configure.xml
elif [ -n "`grep "ro.product.locale=" $1/system/build.prop`" ] ; then
    var=$(grep  "ro.product.locale=" "$1/system/build.prop" )
    echo "<language>${var##"ro.product.locale="}</language>">>$ROOTPATH/configure.xml
elif [ -n "`grep "ro.product.locale.language=" $1/system/build.prop`" ] ; then
    var=$(grep  "ro.product.locale.language=" "$1/system/build.prop" )
    echo "<language>${var##"ro.product.locale.language="}</language>">>$ROOTPATH/configure.xml
else
    echo "<language>en</language>">>$ROOTPATH/configure.xml
fi

#oem
var=$(grep  "ro.fota.oem=" "$1/system/build.prop" )
echo "<oem>${var##"ro.fota.oem="}</oem>">>$ROOTPATH/configure.xml

#operator
echo "<operator>other</operator>">>$ROOTPATH/configure.xml

#model
var=$(grep  "ro.fota.device=" "$1/system/build.prop" )
product=${var##"ro.fota.device="}
echo "<product>$product</product>">>$ROOTPATH/configure.xml

#publishtime
echo "<publishtime>$(date +20%y%m%d%H%M%S)</publishtime>">>$ROOTPATH/configure.xml

#versionname
echo "<versionname>$buildnumber</versionname>">>$ROOTPATH/configure.xml
#key
echo "<key>$2</key>">>$ROOTPATH/configure.xml
echo "</root>">>$ROOTPATH/configure.xml

if [ -f $1/$ROOTPATH.zip ]; then
echo "delete exist file:$1/target_files-package"
rm -f $1/$ROOTPATH.zip
fi

cd $ROOTPATH
zip -rq $ROOTPATH.zip ./
cd ..
mv $ROOTPATH/$ROOTPATH.zip $1/$ROOTPATH.zip
rm -rf $ROOTPATH 

