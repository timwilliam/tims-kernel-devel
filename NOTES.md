# Kernel Development Learning Notes

Original author of the study material is by **Write your own Operating System** on YouTube: [Linux Kernel Programming - YouTube](https://youtube.com/playlist?list=PLHh55M_Kq4OChzSZUHzjjSetgiaTLB0Nz)

The notes here is meant to be my personal learning notes when learning the material.

## Video 01: Compile and Boot

### Step 1: Preparation

Clone the Linux kernel source code from [Linux Kernel Archives](https://www.kernel.org/)

- At the time of writing, I was using kernel `5.16` and was compiling the kernel on `Ubuntu 21.04 Server` running inside VirtualBox

- To get the `git clone` link, click the `browse` button on the kernel version of your liking

- Copy the git tree link on top of the page, it should look something like this:
  `https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/`

- Then you can just `git clone` as follows:
  `git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/`

- After cloning, you can open the kernel code using an IDE and make changes. The file is quite big so it can take a while to load into the IDE. You can use VS Code or KDevelop to edit the kernel code. You can start from around line 800, where you have the function `start_kernel()`.

### Step 2: Package Installation for Kernel Compilation

Before compiling, you need to install some packages. Details on this can also be found in the [Linux Kernel Archives](https://www.kernel.org/) website, in the Documentation section. [There is a list of packages](https://docs.kernel.org/process/changes.html#current-minimal-requirements) that needs to be installed for compiling the kernel there.

```bash
sudo apt install binutils flex bison util-linux \
                 kmod e2fsprogs jfsutils reiserfsprogs \
                 xfsprogs squashfs-tools btrfs-progs \
                 pcmciautils quota ppp nfs-kernel-server \
                 nfs-common procps oprofile udev \
                 grub2-common iptables openssl \
                 libssl-dev bc libelf-dev dwarves
sudo apt install cpio xmlto sphinx-common \
                 perl tar xz-utils
```

### Step 3: Generating the Kernel Config File

Now we move on to the Linux kernel `*.config` file. The Linux kernel has over 11,000 different options. The `*.config` is essential as it is the file that  stores all of your selections for these available options. Without the `*.config` you will not be able to build the Linux kernel.

One way to get it is by running the command `make config`, but this will make you sit through and pick over all of the available options. You have to manually select and set each of the available options. You can also do `make defconfig` where it will set the default value that the developer set for all of the available options. But then the developer does not know your specific hardware you are using, or your Linux distribution. So this is also not the best option.

The best option might be to take the default value from the distribution that you are currently using. You can do this by running the command.

```bash
# cd to your kernel directory
cd /home/tim/devel/myKernel

# location my vary, you can find it in any one of these:
# /proc/config.gz # you can use zcat for this, e.g.: zcat /proc/config.gz > .config
# /boot/config
# /boot/config-$(uname -r)

cp /boot/config-$(uname -r) .config
```

After we get the `*.config`, we can edit it with `make menuconfig`. It will give you text-based tool to edit all your options. Or you can also use `make xconfig`.

Note that we downloaded the kernel version `5.16`, but the `*.config` that we get from our local machine is in different version. You can check the kernel version you are currently running on your machine with the command `uname -a`. So some options might have changed, and some options might not have any entries, and you will probably get error. So what you need to do is first update the current kernel file you just generated to the newest one. You can do this with the command `make oldconfig`. It will ask you about the new options with have been added in the newest version of the kernel.

### Step 4: Compiling the Kernel

After that you can start compiling the kernel. If your compilation fails, it is very likely that you are just missing some packages. So try to install it and compile again. If you compilation fails, **don't rerun** the `make` command. Instead, reset first by using the command `make mrproper`. Note that this will also delete your `.config` file, so remember to make a backup.

```bash
# If you faced some error with CONFIG_X86_X32, change # to renamethe following:
# CONFIG_X86_X32=N
# CONFIG_SYSTEM_TRUSTED_KEYS=""

# Step 1:
make V=1 all -j$(nproc)

# Step 2: Generate the modules and install them
# If you have this error: No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'
# Use one of the below
# scripts/config --disable SYSTEM_TRUSTED_KEYS
# scripts/config --set-str SYSTEM_TRUSTED_KEYS "" # alternative
# Discussion thread: https://askubuntu.com/a/1329625
# If you have this error: No rule to make target 'debian/canonical-revoked-certs.pem' , needed by certs/x509_revocation_list'
# scripts/config --disable SYSTEM_REVOCATION_KEYS
# Discussion thread: https://askubuntu.com/q/1362455
make modules
sudo make modules_install

# Step 3: Install the kernel
sudo make install


# Step 4: Check if everything is installed correctly
make kernelversion # to check kernel version
sudo cp /boot/vmlinuz /boot/vmlinux-$(make kernelversion)-$(uname -m) # rename the newly generated kernel with the current kernel version
```

### Step 5: Update Grub boot loader with the new Kernel and boot

Add a custom menu entry Grub

```bash
sudo vim /etc/grub.d/40_custom
```

Add a new entry for the newly compiled kernel. You can get the default menuentry with the `sudo cat /boot/grub/grub.cfg` and then update with the kernel version yu just compiled before.

```bash
#!/bin/sh
exec tail -n +3 $0
# This file provides an easy way to add custom menu entries.  Simply type the
# menu entries you want to add after this comment.  Be careful not to change
# the 'exec tail' line above.

menuentry 'Tim Ubuntu, with Linux 5.17.0-rc1' --class ubuntu --class gnu-linux --class gnu --class os {
                recordfail
                load_video
                gfxmode $linux_gfx_mode
                insmod gzio
                if [ x$grub_platform = xxen ]; then insmod xzio; insmod lzopio; fi
                insmod part_gpt
                insmod ext2
                set root='hd0,gpt6'
                if [ x$feature_platform_search_hint = xy ]; then
                  search --no-floppy --fs-uuid --set=root --hint-bios=hd0,gpt6 --hint-efi=hd0,gpt6 --hint-baremetal=ahci0,gpt6  18b46609-5be9-47ee-86fd-462a45d779cc
                else
                  search --no-floppy --fs-uuid --set=root 18b46609-5be9-47ee-86fd-462a45d779cc
                fi
                echo    'Loading Linux 5.17.0-rc1 ...'
                linux   /vmlinux-5.17.0-rc1-x86_64 root=UUID=e7efb94c-2afd-4244-827b-ab9766cef225 ro
                echo    'Loading initial ramdisk ...'
                initrd  /initrd.img-5.17.0-rc1+
}
```

Add a timeout for Grub to show the menuentry

```bash
# Open the grub file
sudo vim /etc/default/grub

# Then do the following
# 1: Comment out the line with GRUB_TIMEOUT_STYLE=hidden
# 2: Set the timeout to 5 seconds, GRUB_TIMEOUT=5
```

Finally, update grub with

```bash
sudo update-grub
```

Then you can reboot your machine and select the new menuentry you just created.

Extra note, if you somehow need to update the UUID of the drive, you can use either of the following commands to get the UUID and mount point information.

```bash
cat /etc/mtab
sudo fdisk -l
sudo blkid
df -H
```

Congratulations you have booted to your OS with the kernel that you just compiled! You can  validate this by running `uname -a` when you logged in and you can see that it outputs the kernel version that you just compiled!

## Video 02: Kernel Modules

### Step 1: Install Kernel headers

Drivers are also part of Kernel modules. To write a kernel modules, we will need to have the header files for your kernel. If you have compiled your own Kernel, then you don't have to install the headers. But in case you need to, you can install it with the following with:

```bash
sudo apt install linux-headers-$(uname -r)
```

### Step 2: Write the Kernel module code

Kernel modules are usually written in C, so we create a `.c` file alongside a `Makefile`.

```bash
mkdir hellokernel && cd hellokernel
touch hellokernel.c Makefile
```

As an example, we will use the `hellokernel.c` file:

```c
// Example taken from "The Linux Kernel Module Programming Guide" book

#include <linux/module.h>

int count = 1;
module_param(count, int, 0);
MODULE_PARM_DESC(count, "How many times to print hello");

// We put static here to avoid kernel modules with the same name
// - with static, so that these names are not exported, so no naming collisions
static int __init init_hello(void){
    int i;
    for(i = 0; i < count; i++){
        printk("%d Hello, Kernel!\n", i);
    }
    return 0;
}

static void __exit cleanup_hello(void){
    printk("Goodbye, Kernel!\n");
}

// Incorporate to preprocessor macros
module_init(init_hello);
module_exit(cleanup_hello);

// Kernel might react differently to different licenses
MODULE_LICENSE("GPL");
```

As well as an example of the `Makefile`:

```c
obj-m := hellokernel.o
```

### Step 3: Compile the Kernel module code

We will compile with `make` but not the directly with the one that we wrote in previous step.

```bash
make -C /lib/modules/$(uname -r)/build M=$(pwd)
```

### Step 4: Load the `hellokernel` Kernel module

Load the `hellokernel` kernel module

```bash
sudo insmod ./hellokernel.ko

# or with parameters
sudo insmod ./hellokernel.ko count=3
```

List the loaded kernel module

```bash
sudo lsmod # and look for hellokernel

# or

sudo lsmod | grep "hellokernel"

# sample output:
# hellokernel            16384  0 
```

To unload the module, you can use

```bash
sudo rmmod hellokernel
```

The output of the kernel module will not be printed out directly as it is possible that the kernel module is run on a device without any screen attach to it. Giving it no reason to printout anything. To view the printout result of running the `hellokernel` explicitly, we can use

```bash
sudo dmesg

# sample output
#[ 7689.336867] Hello, Kernel!
#[ 7749.247778] Goodbye, Kernel!
```

 Note that `dmesg` use a ring buffer, so there is a limit on how many messages can be stored and displayed.

### Step 5: Load the Kernel modules on system startup

Note that this assumes that your OS is using `systemd` . To add the `hellokernel` module to system startup, we create a file in

```bash
sudo vim /etc/modules-load.d/hellokernel.conf
```

Then, inside the `hellokernel.conf`, we add

```bash
hellokernel count=1
```

Note that this will load he module with `modprobe`, but it does not know the location of the `hellokernel` so it will fail to load. To solve this, we will need to copy the modules to the proper directory and update the `modprobe` cache

```bash
sudo cp ./hellokernel.ko /lib/modules/$(uname -r)/
sudo depmod -a
```

Then you can try to load the kernel with `modprobe` and it will work

```bash
sudo modprobe hellokernel
```


