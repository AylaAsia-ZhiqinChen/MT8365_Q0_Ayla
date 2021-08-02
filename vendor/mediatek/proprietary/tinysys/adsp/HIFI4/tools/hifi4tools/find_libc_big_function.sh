#!/bin/bash

big_func_list=(
fopen
fclose
feof
fflush
fgetc
fgetpos
fgets
fprintf
fputc
fputs
fread
freopen
fscanf
fseek
fsetpos
ftell
fwrite
gets
sscanf
_Assert
ctime
)

usage() {
    echo "Usage: $0 <map_file>"
    echo ""
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi

map_file=$1
grep_pattern=''
for func in ${big_func_list[@]};
do
	grep_pattern+=$func'|'
done
grep_pattern=${grep_pattern%|}
egrep -w ${grep_pattern} $map_file
if [ $? != 0 ]; then
	exit 0
else
	echo ''
	echo "***************************************"
	echo "Found BIG functions, please check!"
	echo "***************************************"
	echo ''
	exit 1
fi
