#!/bin/sh

if [ -x $ANDROID_PRODUCT_OUT/host/linux-x86/bin/certutil ]
then
  # Use the new build version
  my_certutil=$ANDROID_PRODUCT_OUT/host/linux-x86/bin/certutil
else
  # Use the prebuilt version
  my_certutil=./bin/certutil
fi

mkdir 0_pem_sha1
COUNTER=0
for cert in `ls CERT`
  do
   echo =============================
   echo CERT/${cert}
   echo =============================
   echo --- Method 1 ---
   echo ${my_certutil} x509 -subject_hash -noout -in CERT/${cert}
  if hash_name=`${my_certutil} x509 -subject_hash -noout -in CERT/${cert}`
  then
     echo hash_name=$hash_name
     ${my_certutil} x509 -in CERT/${cert} -out 0_pem_sha1/${hash_name}.0
     ${my_certutil} x509 -in CERT/${cert} -noout -text -fingerprint >> 0_pem_sha1/${hash_name}.0
  else
     echo --- Method 2 instead ---
     echo ${my_certutil} x509 -subject_hash -noout -in CERT/${cert} -inform DER
     if hash_name=`${my_certutil} x509 -subject_hash -noout -in CERT/${cert} -inform DER`
     then
       echo hash_name=$hash_name
       ${my_certutil} x509 -in CERT/${cert} -inform DER -out 0_pem_sha1/${hash_name}.0
       ${my_certutil} x509 -in CERT/${cert} -inform DER -noout -text -fingerprint >> 0_pem_sha1/${hash_name}.0
     else
       echo !!! Method 3 instead !!!
       echo ${my_certutil} x509 -subject_hash -noout -in CERT/${cert} -inform PEM
       if hash_name=`${my_certutil} x509 -subject_hash -noout -in CERT/${cert} -inform PEM`
       then
         echo hash_name=$hash_name
         ${my_certutil} x509 -in CERT/${cert} -inform PEM -out 0_pem_sha1/${hash_name}.0
         ${my_certutil} x509 -in CERT/${cert} -inform PEM -noout -text -fingerprint >> 0_pem_sha1/${hash_name}.0
       fi
     fi
  fi
done
