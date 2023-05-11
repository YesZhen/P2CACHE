#!/bin/bash

cores=$1
image=$2 # optional

if [ ! -f $image ]; then 
    echo "$image does not exist. Please run flytrap/tools/create-image.sh and move focal.img to the flytrap root directory."
fi


if [ -z $cores ]; then
    echo "Usage: boot-vm.sh cores [image number]"
    exit 1
fi

image_name="focal${image}.img"

# sudo qemu-system-x86_64 -boot c -m 8192 -hda $image -enable-kvm \
# -nographic -kernel share/linux-5.1/arch/x86/boot/bzImage -append \
# "root=/dev/sda console=ttyS0 earlyprintk=serial memmap=128M!4G memmap=128M!4224M" \
# -fsdev local,security_model=passthrough,id=fsdev0,path=share -device virtio-9p-pci,\
# id=fs0,fsdev=fsdev0,mount_tag=hostshare -smp $cores -net nic -net user,\
# hostfwd=tcp::2222-:22 -cpu host
sudo numactl -N 1 qemu-system-x86_64 -boot c -hda $image_name -enable-kvm \
-nographic -kernel share/linux-5.4.225/arch/x86/boot/bzImage -append \
"root=/dev/sda console=ttyS0 earlyprintk=serial nmi_watchdog=0" \
-fsdev local,security_model=passthrough,id=fsdev0,path=share$image -device virtio-9p-pci,\
id=fs0,fsdev=fsdev0,mount_tag=hostshare -smp $cores,sockets=1,cores=$cores -cpu host -net nic -net user,\
hostfwd=tcp::2222-:22 -cpu host \
-m 240G,slots=20,maxmem=800G \
-machine pc,nvdimm=on \
-object memory-backend-file,id=mem0,share=on,mem-path=/dev/dax2.0,size=480G,align=16M \
-device nvdimm,id=nv0,memdev=mem0,label-size=128K \
-hdc /dev/sdb

# memmap=128G!64G