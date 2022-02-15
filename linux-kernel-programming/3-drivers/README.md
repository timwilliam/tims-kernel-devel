# Video 03: Drivers

This video teaches you how to create a simple driver module that allows the userspace to interact with underling hardware.

## Step 1: Prepare the hardware

## Step 2: Compile the driver code

Compiing the driver code can be run with the following command:

```bash
make -C /lib/modules/$(uname -r)/build "M=$PWD"
```

## Step 3: Run the driver code

Then you can run the code by loading it as a kernel module:

```bash
# load the kernel module
sudo insmod ./hellodriver.ko

# load the kernel module with custom gpio pin number
sudo insmod ./hellodriver.ko gpio_pin=22

# assume that we use major number 137, and minor number 0
sudo mknod /dev/gpio_morse c 137 0
sudo chmod ugo+rw /dev/gpio_morse

# and then you can write to that device and you will hear the morse sound
echo "SMS" > /dev/gpio_morse
```

To stop, and remove, you can use:

```bash
# stop the loaded kernel module
sudo rmmod hellodriver

# remove the device
sudo rm /dev/gpio_morse
```


