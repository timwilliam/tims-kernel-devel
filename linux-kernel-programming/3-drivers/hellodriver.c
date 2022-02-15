#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h> // for timing and stuff

#include <linux/fs.h> // for file system
#include <linux/cdev.h> // for character device
#include <linux/io.h> // for input output

// set gpio_pin as parameter so that we can change it later on
int gpio_pin = 17;
module_param(gpio_pin, int, 0);
MODULE_PARM_DESC(gpio_pin, "The GPIO Pin-ID that we send the data to");

int timescale = 100; // 100ms
module_param(timescale, int, 0);
MODULE_PARM_DESC(timescale, "The number of milliseconds for one time-unit (default is 100ms)");

void morse_sound(int value, int time){
    gpio_set_value(gpio_pin, value);
    msleep(time * timescale);
}

void dit(void){
    morse_sound(1, 1);
    morse_sound(0, 1);
}

void dah(void){
    morse_sound(1, 3);
    morse_sound(0, 1);
}

void morse(char *str){
    char *s;
    for(s = str; *s; ++s){
        char c = *s;
        if('a' <= c && c <= 'z'){
            c = c - 'a' +'A';
        }

        switch(*s){
            case 'A':	dit(); dah(); break;
            case 'B':	dah(); dit(); dit(); dit(); break;
            case 'C':	dah(); dit(); dah(); dit(); break;
            case 'D':	dah(); dit(); dit(); break;
            case 'E':	dit(); break;
            case 'F':	dit(); dit(); dah(); dit(); break;
            case 'G':	dah(); dah(); dit(); break;
            case 'H':	dit(); dit(); dit(); dit(); break;
            case 'I':	dit(); dit(); break;
            case 'J':	dit(); dah(); dah(); dah(); break;
            case 'K':	dah(); dit(); dah(); break;
            case 'L':	dit(); dah(); dit(); dit(); break;
            case 'M':	dah(); dah(); break;
            case 'N':	dah(); dit(); break;
            case 'O':	dah(); dah(); dah(); break;
            case 'P':	dit(); dah(); dah(); dit(); break;
            case 'Q':	dah(); dah(); dit(); dah(); break;
            case 'R':	dit(); dah(); dit(); break;
            case 'S':	dit(); dit(); dit(); break;
            case 'T':	dah(); break;
            case 'U':	dit(); dit(); dah(); break;
            case 'V':	dit(); dit(); dit(); dah(); break;
            case 'W':	dit(); dah(); dah(); break;
            case 'X':	dah(); dit(); dit(); dah(); break;
            case 'Y':	dah(); dit(); dah(); dah(); break;
            case 'Z':	dah(); dah(); dit(); dit(); break;
            case '1':	dit(); dah(); dah(); dah(); dah(); break;
            case '2':	dit(); dit(); dah(); dah(); dah(); break;
            case '3':	dit(); dit(); dit(); dah(); dah(); break;
            case '4':	dit(); dit(); dit(); dit(); dah(); break;
            case '5':	dit(); dit(); dit(); dit(); dit(); break;
            case '6':	dah(); dit(); dit(); dit(); dit(); break;
            case '7':	dah(); dah(); dit(); dit(); dit(); break;
            case '8':	dah(); dah(); dah(); dit(); dit(); break;
            case '9':	dah(); dah(); dah(); dah(); dit(); break;
            case '0':	dah(); dah(); dah(); dah(); dah(); break;
            case '.':	dit(); dah(); dit(); dah(); dit(); dah(); break;
            case ',':	dah(); dah(); dit(); dit(); dah(); dah(); break;
        }

        morse_sound(0, 2);
    }
}

static int open_dev(struct inode *node, struct file *f){
    // you can do checks, or actually connecting to the devicess
    return 0;
}

static int close_dev(struct inode *node, struct file *f){
    // you can do checks, or actually disconnecting to the devicess
    return 0;
}

static ssize_t write_dev(struct file *f, const char *buff, size_t len, loff_t *off){
    unsigned char localbuffer[129];
    unsigned long count;
    
    // this function returns the number of character left, or the one that it could not copy; so a value of 0 means everything is copied
    count = copy_from_user(&localbuffer[0], buff, (len < 128) ? len : 128);
    count = ((len < 128) ? len : 128) - count;
    localbuffer[count] = '\0';
    
    printk("Morse module received data: %s", &localbuffer[0]);
    
    morse(&localbuffer[0]);
    
    return len;
}

struct file_operations my_fops = {
    // pointers to function
    write: write_dev,
    open: open_dev,
    release: close_dev,
    owner: THIS_MODULE
};

struct cdev my_cdev;
dev_t devno;

static int __init init_hellodriver(void){
    int err;

    if(!gpio_is_valid(gpio_pin)){
        printk("Invalid GPIO pin \"%d\"!\n", gpio_pin);
        return -1;
    }
    
    if(gpio_request(gpio_pin, "MORSE_GPIO") < 0){
        printk("Request for GPIO \"%d\" failed!\n", gpio_pin);
        return -1;
    }

    // configure gpio_pin as an output pin
    gpio_direction_output(gpio_pin, 0);

    // for testing, this is actually the default ringtone for Nokia phones when you received an SMS!!!
    // morse("SMS\0");
    
    // get major and minor number for devices
    // $ cat /proc/devices
    // pick an unused number
    devno = MKDEV(137, 0);
    register_chrdev_region(devno, 1, "gpio_morse");

    // associate our function with a character device
    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;

    // give the character device to the kernel, and associate with major and minor number, and create 1 of this object
    err = cdev_add(&my_cdev, devno, 1);
    if(err < 0){
        printk("Error when adding device!");
        return -1;
    }

    return 0;
}

static void __exit cleanup_hellodriver(void){
    gpio_set_value(gpio_pin, 0);
    gpio_free(gpio_pin);
    
    unregister_chrdev_region(devno, 1);
}

module_init(init_hellodriver);
module_exit(cleanup_hellodriver);

MODULE_LICENSE("GPL");
