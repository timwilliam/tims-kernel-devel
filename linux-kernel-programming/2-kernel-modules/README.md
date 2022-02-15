## Video 02: Kernel Modules

This video teaches you how to create a simple kernel module and run it on your system.

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
