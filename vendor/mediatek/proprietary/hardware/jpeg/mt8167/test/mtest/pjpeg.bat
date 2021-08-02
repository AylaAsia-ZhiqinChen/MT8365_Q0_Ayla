copy B:\gitTable\gitM8163-m0-tk-1014\out\target\product\tb8163p3_64\obj\EXECUTABLES\mhalJpegTest_intermediates\LINKED\mhalJpegTest
echo off
adb remount
adb shell rm         /data/jpegtest/*
adb shell mkdir         /data/jpegtest
adb push .\mhalJpegTest  /data/jpegtest/
adb shell  chmod 777    /data/jpegtest/mhalJpegTest

adb push .\test.jpg   /data/jpegtest/test.jpg
echo "Decode JPEG"
adb shell /data/jpegtest/mhalJpegTest 1 test.jpg 1  1 > log.txt
adb pull /data/jpegtest/tg_out_1280_720.yuv
adb pull /data/jpegtest/t1_out_1280_720.yuv
adb pull /data/jpegtest/t2_out_1280_720.yuv
pause

