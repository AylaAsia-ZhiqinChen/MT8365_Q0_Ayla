export TEMPORARY_DISABLE_PATH_RESTRICTIONS=true
if [ "$BUILD_NUMBER" = "" ] ; then
  if [ -f "vendor/mediatek/proprietary/buildinfo/label.ini" ] ; then
    BUILD_NUMBER=$(python device/mediatek/build/buildnumber.py)
    export BUILD_NUMBER=$BUILD_NUMBER
  fi
fi