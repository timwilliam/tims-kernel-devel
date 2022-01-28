#!/bin/bash

KERNEL_VERSION=5.16.3
BUSYBOX_VERSION=1.35.0

mkdir -p src
cd src
    # get the linux kernel, and then compile it
    KERNEL_MAJOR=$(echo $KERNEL_VERSION | sed 's/\([0-9]*\)[^0-9].*/\1/')
    wget https://mirrors.edge.kernel.org/pub/linux/kernel/v$KERNEL_MAJOR.x/linux-$KERNEL_VERSION.tar.xz
    tar -xf linux-$KERNEL_VERSION.tar.xz
    cd linux-$KERNEL_VERSION
        make defconfig
        make -j$(nproc) || exit
    cd ..

    # get busybox, and then build it
    wget https://busybox.net/downloads/busybox-$BUSYBOX_VERSION.tar.bz2
    tar -xf busybox-$BUSYBOX_VERSION.tar.bz2
    cd busybox-$BUSYBOX_VERSION
        make defconfig
        sed 's/^.*CONFIG_STATIC[^_].*$/CONFIG_STATIC=y/g' -i .config
        make -j$(nproc) || exit
    cd ..
cd ..

cp ./src/linux-$KERNEL_VERSION/arch/x86_64/boot/bzimage ./

mkdir initrd
cd initrd
    # /bin is where we would put our busybox binary
    # /dev for device, /proc for process, /sys for interaction with the kernel
    mkdir -p bin dev proc sys
    cd bin
        cp ../../src/busybox-$BUSYBOX_VERSION/busybox ./

        for prog in $(./busybox --list); do
            ln -s /bin/busybox ./$prog
        done
    cd ..
    
    echo '#!/bin/sh' > init
    echo 'mount -t sysfs sysfs /sys' >> init
    echo 'mount -t proc proc /proc' >> init
    echo 'mount -t devtmpfs udev /dev' >> init    
    echo 'sysctl -w kernel.printk="2 4 1 7"' >> init # for logging level
    echo '/bin/sh' >> init
    echo 'poweroff -f' >> init
    
    chmod -R 777 .
    
    find . | cpio -o -H newc > ../initrd.img
cd ..

# to run using qemu
# sudo apt install -y qemu-system-x86_64
# qemu-system-x86_64 -kernel bzImage -initrd initrd.img

# to run inside your current terminal using serial interface (communicate with stdin and stdout)
qemu-system-x86_64 -kernel bzImage -initrd initrd.img -nographic -append 'console=ttyS0'