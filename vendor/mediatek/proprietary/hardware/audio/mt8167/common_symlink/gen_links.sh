#!/bin/bash

find . -type l -delete
find . -type d -delete

ln -s ../../common/aud_drv/ .
ln -s ../../common/speech_driver/ .
ln -s ../../common/utility/ .
ln -s ../../common/V3/ .

