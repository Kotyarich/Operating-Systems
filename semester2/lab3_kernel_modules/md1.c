#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");

char *md1_export_string = "important string";
int md1_export_int = 999;

EXPORT_SYMBOL(md1_export_string);
EXPORT_SYMBOL(md1_export_int);

extern char *md1_is_odd(int n) {
    printk(KERN_INFO "MY_MODULE: calling md1_is_odd\n");
    if (n % 2) {
        return  "n is odd\n";
    }
    return "n is eval\n";
}

extern int md1_factorial(int n) {
    printk(KERN_INFO "MY_MODULE: calling factorial\n");
    int i, res = 1;


    for (i = 1; i <= n; i++) {
        res *= i;
    }

    return res;
}

extern int md1_zero(void) {
    printk(KERN_INFO "MY_MODULE: calling zero\n");
    return 0;
}

EXPORT_SYMBOL(md1_is_odd);
EXPORT_SYMBOL(md1_factorial);
EXPORT_SYMBOL(md1_zero);

static int __init init(void) {
    printk(KERN_INFO "MY_MODULE: md1 init\n");
    return 0;
}

static void __exit exit(void) {
    printk(KERN_INFO "MY_MODULE: md1 exit\n");
}

module_init(init);
module_exit(exit);
