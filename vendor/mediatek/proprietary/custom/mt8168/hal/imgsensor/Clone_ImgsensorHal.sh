#!/bin/sh
# we have only 2 arguments. Print the help text: 
if [ $# -ne 2 ] ; then 
cat 
USAGE: ren 'regexp' 'replacement' files... 
EXAMPLE: clone imgsensor AAA to BBB: 
  clone_proj 'AAA' 'BBB'
HELP 
  exit 0 
fi 

old=$1
new=$2

old_name=${old%_mipi_raw*}
echo "OLD NAME IS $old_name"
new_name=${new%_mipi_raw*}
echo "NEW NAME IS $new_name"

OLD_ONE=$(echo $old_name|tr -t '[a-z]' '[A-Z]')
NEW_ONE=$(echo $new_name|tr -t '[a-z]' '[A-Z]')
echo  OLD ONE IS $OLD_ONE
echo  NEW ONE IS $NEW_ONE

if [ -f "${old}" ];then
	cp $old $new
	echo "clone $new ---OK!"
fi

if [ -d "${old}" ];then
	cp -r $old $new
	echo "clone $new ---OK!"
fi	


cd $new

pwd

rename -v "s/$old_name/$new_name/" *.h
rename -v "s/$old_name/$new_name/" */*.h
rename -v "s/$old_name/$new_name/" */*/*.h

rename -v "s/$old_name/$new_name/" *.cpp
rename -v "s/$old_name/$new_name/" */*.cpp
rename -v "s/$old_name/$new_name/" */*/*.cpp

grep $old_name -rl . |xargs sed -i "s/$old_name/$new_name/g"
grep $OLD_ONE -rl . |xargs sed -i "s/$OLD_ONE/$NEW_ONE/g"











