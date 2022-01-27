# Kernel Development Learning Notes

Original author of the study material is by **Write your own Operating System** on YouTube: [Linux Kernel Programming - YouTube](https://youtube.com/playlist?list=PLHh55M_Kq4OChzSZUHzjjSetgiaTLB0Nz)

The notes here is meant to be my personal learning notes when learning the material.

## Video 01: Compile and Boot

### Step 1: Preparation

Clone the Linux kernel source code from [Linux Kernel Archives](https://www.kernel.org/)

- At the time of writing, I was using kernel `5.16` and was compiling the kernel on `Ubuntu 21.04 Server` running inside VirtualBox

- To get the `git clone` link, click the `browse` button on the kernel version of your liking

- Copy the git tree link on top of the page, it should look something like this:
  
  ```bash
  https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/
  ```

- Then you can just `git clone` as follows:
  
  ```bash
  git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/
  ```

- After cloning, you can open the kernel code using an IDE and make changes. The file is quite big so it can take a while to load into the IDE. You can use VS Code or KDevelop to edit the kernel code. You can start from around line 800, where you have the function `start_kernel()`.

Upon compilation, I face some issues where the storage partition that I set for the VM is not large enough and cause *insufficient space error*. So, I created a VM with the following disk configuration. Note that I make each partition quite big since I have sme free space in my drive and to just avoid and *insufficient space error*.

```bash
/        30G # used around 14G after kernel compilation
/boot    10G # used around 1.3G after kernel compilation
/home    40G # used around 26G after kernel compilation
/var     10G # used around 0.9G after kernel compilation
/tmp     10G # used around 0.1G after kernel compilation

# in total, I used 100GB of disk space for the partitions
```

### Step 2: Package Installation for Kernel Compilation

Before compiling, you need to install some packages. Details on this can also be found in the [Linux Kernel Archives](https://www.kernel.org/) website, in the Documentation section. [There is a list of packages](https://docs.kernel.org/process/changes.html#current-minimal-requirements) that needs to be installed for compiling the kernel there.

```bash
sudo apt install binutils flex bison util-linux \
                 kmod e2fsprogs jfsutils reiserfsprogs \
                 xfsprogs squashfs-tools btrfs-progs \
                 pcmciautils quota ppp nfs-kernel-server \
                 nfs-common procps oprofile udev \
                 grub2-common iptables openssl \
                 libssl-dev bc libelf-dev dwarves \
                 cpio xmlto sphinx-common \
                 perl tar xz-utils
```

### Step 3: Generating the Kernel Config File

Now we move on to the Linux kernel `*.config` file. The Linux kernel has over 11,000 different options. The `*.config` is essential as it is the file that  stores all of your selections for these available options. Without the `*.config` you will not be able to build the Linux kernel.

One way to get it is by running the command `make config`, but this will make you sit through and pick over all of the available options. You have to manually select and set each of the available options. You can also do `make defconfig` where it will set the default value that the developer set for all of the available options. But then the developer does not know your specific hardware you are using, or your Linux distribution. So this is also not the best option.

The best option might be to take the default value from the distribution that you are currently using. You can do this by running the command.

```bash
# cd to your kernel directory
cd /devel/linux

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
