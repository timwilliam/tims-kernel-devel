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
sudo apt install inutils flex bison util-linux kmod e2fsprogs jfsutils reiserfsprogs xfsprogs squashfs-tools btrfs-progs pcmciautils quota ppp nfs-kernel-server nfs-common procps oprofile udev grub2-common iptables openssl libssl-dev bc libelf-dev
sudo apt install cpio xmlto sphinx-common perl tar xz-utils
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

After that you can start compiling the kernel with the command`make -j$(nproc)`. If your compilation fails, it is very likely that you are just missing some packages. So try to install it and compile again. If you compilation fails, **don't rerun** the `make` command. Instead, reset first by using the command `make mrproper`. Note that this will also delete your `.config` file, so remember to make a backup.

After a successful compilation, we run the command `sudo make modules_install`. 

Some additional notes up till this point. I personally faced some miscellanous issues when following the step as described in the video, below is what I personally run and resulted in a success.

```bash
# In the .config file, I have to change the following
# CONFIG_X86_X32=N
# CONFIG_SYSTEM_TRUSTED_KEYS=""

# According to this thread, you should run the following before install
# https://stackoverflow.com/a/64462989https://stackoverflow.com/a/64462989

make V=1 all -j$(nproc)
sudo make modules
sudo make modules_install

# If you have this error: No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'
# Use one of the below
# scripts/config --disable SYSTEM_TRUSTED_KEYS
# scripts/config --set-str SYSTEM_TRUSTED_KEYS "" # alternative
# Discussion thread: https://askubuntu.com/a/1329625

# If you have this error: No rule to make target 'debian/canonical-revoked-certs.pem' , needed by certs/x509_revocation_list'
# scripts/config --disable SYSTEM_REVOCATION_KEYS
# Discussion thread: https://askubuntu.com/q/1362455
```




