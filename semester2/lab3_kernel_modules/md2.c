#include <linux/init.h>
#include <linux/module.h>
#include "md1.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");

static int __init init(void) {
    printk(KERN_INFO "MY_MODULE: md2 init\n");

    printk(KERN_INFO "MY_MODULE: md1_export_string: %s\n", md1_export_string);
    printk(KERN_INFO "MY_MODULE: md1_export_int: %d\n", md1_export_int);

    printk(KERN_INFO "MY_MODULE: md1_is_odd 4: %s\n", md1_is_odd(4));
    printk(KERN_INFO "MY_MODULE: md1_is_odd 11: %s\n", md1_is_odd(11));
    printk(KERN_INFO "MY_MODULE: factorial 5: %d\n", md1_factorial(5));
    printk(KERN_INFO "MY_MODULE: zero: %d\n", md1_zero());

    return 0;
}

static void __exit exit(void) {
    printk(KERN_INFO "MY_MODULE: md2 exit\n");
}

module_init(init);
module_exit(exit);
