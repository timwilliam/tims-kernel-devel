// Example taken from "The Linux Kernel Module Programming Guide" book

#include <linux/module.h>

int count = 1;
module_param(count, int, 0);
MODULE_PARM_DESC(count, "How many times to print hello");

// We put static here to avoid kernel modules with the same name
// - with static, so that these names are not exported, so no naming collisions
static int __init init_hello(void){
    int i;
    for(i = 1; i <= count; i++){
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
