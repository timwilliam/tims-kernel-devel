# Video 03: Drivers

This video teaches you how to create a simple driver module that allows the userspace to interact with underling hardware.

Video Link:

- [Linux Kernel Programming 02.5: Preparations - YouTube](https://youtu.be/ivPZwIqsP84)

- [Linux Kernel Programming 03: Drivers - YouTube](https://youtu.be/catF6dSSzag)

## Step 1: Prepare the hardware

We will be using a RaspberryPi for simulating a device, capable to control external hardware attached to it. In My case, I am using a RaspberryPi 1 Model B.

First we will need to compile our own kernel for the Raspberry Pi. You can follow the official tutorial made available in the link below. Note that this takes about 1 days of building for the RaspberryPi I'm using.

- [Raspberry Pi Documentation - The Linux kernel](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#building-the-kernel)

After that, we can attach the hardware to the RaspberryPi. We will need a breadboard, a small buzzer, a 220Ω resistor, and some jumper cables. Simply connect the small buzzer and 220Ω in series on the breadboard, and plug it to GPIO17 (this is the pin we pick for the example) of the RaspberryPi. I don't have the schematic but below is a screenshot from the video tutorial of how this will look like on the breadboard.

![Hardware Setup](hardware-setup.png "Hardware Setup")

Then, we are ready to move on to the next step in the video tutorial.

## Step 2: Compile the driver code

Compiling the driver code can be run with the following command:

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
