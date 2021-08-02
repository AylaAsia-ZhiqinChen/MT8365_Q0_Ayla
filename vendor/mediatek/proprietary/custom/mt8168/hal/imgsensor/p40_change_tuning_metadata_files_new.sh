# it's a basic script tool for changing a tuning file with a new name
# eg: ov5648mipiraw --> ov2680mipiraw
# Notice:  
#   0 This is for build pass only.
#   1 You should tuning them for future;

# change_tuning_files.sh ov5648_mipi_raw ov2680_mipi_raw

# get src and dest tuning name
if [ $# -lt 2 ]; 
then
  echo "Arguments missing..."
  # TODO: show help
  echo "`basename $0` <src_name> <dest_name>"
  echo "  eg: `basename $0` ov5648_mipi_raw ov2680_mipi_raw"
  echo "  Please place the file on the same folder of <src_name>"
  exit -1
fi

SRC=$1  	# like: ov5648_mipi_raw
DEST=$2 	# like: ov2680_mipi_raw
echo "src:$SRC, dest:$DEST"
SRC=`echo $SRC |  tr [A-Z] [a-z]`
DEST=`echo $DEST |  tr [A-Z] [a-z]`
echo "Convert to lower case: src:$SRC, dest:$DEST"

SRC_1=`echo $SRC | tr -d _`		# like: ov5648mipiraw
DEST_1=`echo $DEST | tr -d _`	# like: ov2680mipiraw
echo "src1:$SRC_1, dest1:$DEST_1"

# seperate SRC & DEST for detail processing
SRC_SEN_NAME=`echo $SRC | cut -d_ -f 1`		# like: ov5648
SRC_SEN_TYPE=`echo $SRC | cut -d_ -f 2,3`   # like: mipi_raw, raw

DEST_SEN_NAME=`echo $DEST | cut -d_ -f 1`		# like: ov2680
DEST_SEN_TYPE=`echo $DEST | cut -d_ -f 2,3`   	# like: mipi_raw, raw
echo "SRC_SEN_NAME:$SRC_SEN_NAME, DEST_SEN_NAME:$DEST_SEN_NAME"

# upper case of src sensor name
SRC_SEN_NAME_UPPER=`echo $SRC_SEN_NAME | tr [a-z] [A-Z]`	# like: OV5648
# upper case of dest sensor name
DEST_SEN_NAME_UPPER=`echo $DEST_SEN_NAME | tr [a-z] [A-Z]`	# like: OV2680
echo "SRC_SEN_NAME_UPPER:$SRC_SEN_NAME_UPPER, DEST_SEN_NAME_UPPER:$DEST_SEN_NAME_UPPER"

# copy a new folder
cp -r $SRC $DEST

# change file name
cd $DEST

rename  "s/$SRC_1/$DEST_1/g" ./*

rename  "s/$SRC_1/$DEST_1/g" ./AE_Tuning_Para/*
rename  "s/$SRC_1/$DEST_1/g" ./AutoHDR_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./AutoHDR_Preview/*
rename  "s/$SRC_1/$DEST_1/g" ./AutoHDR_Video/*
#rename  "s/$SRC_1/$DEST_1/g" ./AWB_Tuning_Para/*
rename  "s/$SRC_1/$DEST_1/g" ./Face_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./Flash_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./Flash_HDR_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./HDR_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./HDR_Preview/*
rename  "s/$SRC_1/$DEST_1/g" ./HDR_Video/*
rename  "s/$SRC_1/$DEST_1/g" ./N3D_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./N3D_Preview/*
rename  "s/$SRC_1/$DEST_1/g" ./N3D_Video/*
rename  "s/$SRC_1/$DEST_1/g" ./Reserved/*
rename  "s/$SRC_1/$DEST_1/g" ./Scene_Capture/*
rename  "s/$SRC_1/$DEST_1/g" ./Scene_Capture_4cell/*
rename  "s/$SRC_1/$DEST_1/g" ./Scene_Preview/*
rename  "s/$SRC_1/$DEST_1/g" ./Scene_Preview_4k/*
rename  "s/$SRC_1/$DEST_1/g" ./Scene_Preview_ZSD_Flash/*
rename  "s/$SRC_1/$DEST_1/g" ./tuning_mapping/*
rename  "s/$SRC_1/$DEST_1/g" ./Video_1080/*
rename  "s/$SRC_1/$DEST_1/g" ./Video_4k/*

cd ..

# change file contents
cd $DEST

sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./AE_Tuning_Para/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./AutoHDR_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./AutoHDR_Preview/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./AutoHDR_Video/*
#sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./AWB_Tuning_Para/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Face_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Flash_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Flash_HDR_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./HDR_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./HDR_Preview/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./HDR_Video/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./N3D_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./N3D_Preview/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./N3D_Video/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Reserved/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Scene_Capture/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Scene_Capture_4cell/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Scene_Preview/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Scene_Preview_4k/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Scene_Preview_ZSD_Flash/*
#sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" sensor_info.mk
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" Android.mk
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./tuning_mapping/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Video_1080/*
sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./Video_4k/*

sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./AE_Tuning_Para/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./AutoHDR_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./AutoHDR_Preview/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./AutoHDR_Video/*
#sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./AWB_Tuning_Para/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Face_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Flash_Capture/*
#sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Flash_Capture/*
#sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Face_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Flash_HDR_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./HDR_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./HDR_Preview/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./HDR_Video/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./N3D_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./N3D_Preview/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./N3D_Video/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Reserved/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Scene_Capture/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Scene_Capture_4cell/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Scene_Preview/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Scene_Preview_4k/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Scene_Preview_ZSD_Flash/*
#sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" sensor_info.mk
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" Android.mk
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./tuning_mapping/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Video_1080/*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./Video_4k/*

sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./camera*
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./camera*

sed -i "s/$SRC_SEN_NAME/$DEST_SEN_NAME/g" ./${DEST_1}_Info.h
sed -i "s/$SRC_SEN_NAME_UPPER/$DEST_SEN_NAME_UPPER/g" ./${DEST_1}_Info.h

cd ..
