# Video 01: Hello World Linux Kernel Module

This video gives you an introduction of linux kernel module programming.

Video Link:

- [Let's code a Linux Driver - 1: Hello World Linux Kernel Module - YouTube](https://youtu.be/4tgluSJDA_E)



A Linux Kernel module is a piece of software that runs in the kernel space. With this, it has advantages and disadvatages. 

Advantages:

- Allow for direct access to hardware

- Able to be loaded dynamically into the kernel and out of the kernel

Disadvantages:

- Some variable like `float` is not available out of the box

- The stack is a little bit smaller



To compile and run the code do the following:

```bash
# compile and generate the kernel module files
make

# insert the module
sudo insmod ./mymodule.ko

# check if the module is running
lsmod | grep mymodule

# check the kernel log
# sample output: [80126.807838] Hello, Kernel!
dmesg | tail

# to remove the kernel module
rmmod mymodule

# check the kernel log again after removing the module
# sample output: [80160.332005] Goodbye, Kernel!
dmesg | tail
```




