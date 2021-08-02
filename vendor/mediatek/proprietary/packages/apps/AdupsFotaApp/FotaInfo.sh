#!/bin/bash

#********Do not modify this file. If you want modify this file, pls contact our FAE*******
echo ""
echo "# begin fota properties"
echo "ro.fota.platform=MTK8365_10.0"
#type info: phone, pad ,box, tv
echo "ro.fota.type=phone"
echo "ro.fota.app=5"
echo "ro.fota.battery=30"
#ro.fota.hide.shake
echo "ro.fota.hide.shake=1"
echo "ro.boot.vendor.overlay.theme=com.adups.fota.overlay"
#oem info
echo "ro.fota.oem=yuntian8365_10.0"
#model info, Settings->About phone->Model number
if [ -n "`grep "ro.product.model=" $1`" ] ; then
FotaDevice=$(grep "ro.product.model=" "$1"|awk -F "=" '{print $NF}' )
elif [ -n "`grep "ro.product.system.model=" $1`" ] ; then
FotaDevice=$(grep "ro.product.system.model=" "$1"|awk -F "=" '{print $NF}' )
elif [ -n "`grep "ro.product.system.device=" $1`" ] ; then
FotaDevice=$(grep "ro.product.system.device=" "$1"|awk -F "=" '{print $NF}' )
else
echo "ERROR: model do not exist in system/build.prop,Please define it"
exit 0
fi
echo "ro.fota.device=$FotaDevice" | sed  's/[^0-9a-zA-Z= ._-]//g'
#version number, Settings->About phone->Build number
#FotaVersion=$(grep "ro.build.display.id=" "$1"|awk -F "=" '{print $NF}' )
FotaVersion=$(grep "ro.build.display.id=" "$1"|awk -F "=" '{print $NF}' )`date +_%Y%m%d-%H%M`
echo "ro.fota.version=$FotaVersion" | sed  's/[^0-9a-zA-Z= ._-]//g'
echo "# end fota properties"
#fota other 
#ro.fota.no_touch
#echo "ro.fota.no_touch=1"
#imei ,sn, mac
echo "ro.fota.id=sn"
#display:0:vertical，1:horizontal，2:vertical or horizontal
echo "ro.fota.display=1"
#ro.fota.fmcheck
#action:com.adups.fota.OUT_BROADCAST_NEWVERSION ,permission:com.adups.fota.permission
#echo "ro.fota.fmcheck=1"
#********Do not modify this file. If you want modify this file, pls contact our FAE*******
