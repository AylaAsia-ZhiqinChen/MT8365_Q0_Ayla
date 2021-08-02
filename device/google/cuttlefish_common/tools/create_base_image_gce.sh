#!/bin/bash

set -x
set -o errexit

sudo apt-get update

# Stuff we need to get build support

sudo apt install -y debhelper ubuntu-dev-tools equivs "${extra_packages[@]}"

# Install the cuttlefish build deps

for dsc in *.dsc; do
  yes | sudo mk-build-deps -i "${dsc}" -t apt-get
done

# Installing the build dependencies left some .deb files around. Remove them
# to keep them from landing on the image.
yes | rm -f *.deb

for dsc in *.dsc; do
  # Unpack the source and build it

  dpkg-source -x "${dsc}"
  dir="$(basename "${dsc}" .dsc)"
  dir="${dir/_/-}"
  pushd "${dir}/"
  debuild -uc -us
  popd
done

# Now gather all of the *.deb files to copy them into the image
debs=(*.deb)

tmp_debs=()
for i in "${debs[@]}"; do
  tmp_debs+=(/tmp/"$(basename "$i")")
done

# Now install the packages on the disk
sudo mkdir /mnt/image
sudo mount /dev/sdb1 /mnt/image
cp "${debs[@]}" /mnt/image/tmp
sudo mount -t sysfs none /mnt/image/sys
sudo mount -t proc none /mnt/image/proc
sudo mount --bind /dev/ /mnt/image/dev
sudo mount --bind /dev/pts /mnt/image/dev/pts
sudo mount --bind /run /mnt/image/run
# resolv.conf is needed on Debian but not Ubuntu
sudo cp /etc/resolv.conf /mnt/image/etc/
sudo chroot /mnt/image /usr/bin/apt update
sudo chroot /mnt/image /usr/bin/apt install -y "${tmp_debs[@]}"
# install tools dependencies
sudo chroot /mnt/image /usr/bin/apt install -y default-jre
sudo chroot /mnt/image /usr/bin/apt install -y unzip bzip2
sudo chroot /mnt/image /usr/bin/apt install -y aapt

sudo chroot /mnt/image /usr/bin/find /home -ls
# Clean up the builder's version of resolv.conf
sudo rm /mnt/image/etc/resolv.conf

# Skip unmounting:
#  Sometimes systemd starts, making it hard to unmount
#  In any case we'll unmount cleanly when the instance shuts down
echo IMAGE_WAS_CREATED
