#!/bin/sh
if [ $# != 1 ]; then
    echo "Usage:$0 [hd devnum /dev/sd<x>]\n"
    exit 0
fi

DEVNAME="$1"
HDSIZE=$(sudo parted -s $DEVNAME print | grep $DEVNAME | awk '{print $3}')
FIRSTP=$DEVNAME"1"

sudo parted -s $DEVNAME mklabel gpt
sudo parted -s $DEVNAME mkpart primary 0 $HDSIZE
sudo mkfs.ntfs -f $FIRSTP

echo "sdb="$P2

