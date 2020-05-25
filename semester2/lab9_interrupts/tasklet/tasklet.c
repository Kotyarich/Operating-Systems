#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");
MODULE_DESCRIPTION("tasklet");

#define IRQ 1
static int my_dev_id;

char tasklet_data[] = "tasklet was called";

void tasklet(unsigned long data);

DECLARE_TASKLET(my_tasklet, tasklet, (unsigned long) &tasklet_data);

void tasklet(unsigned long data) {
    printk(KERN_INFO "MY_TASKLET state: %ld, count: %d, data: %s\n",
        my_tasklet.state,
        my_tasklet.count,
        my_tasklet.data);
}

static irqreturn_t my_interrupt(int irq, void *dev_id) {
    if (irq == IRQ) {
        printk(KERN_INFO "MY_INTERRUP calling tasklet_schedule\n");
        tasklet_schedule(&my_tasklet);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static int __init my_tasklet_init(void) {
    if (request_irq(IRQ, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id)) {
        return -1;
    }
    printk(KERN_INFO "MY_INTERRUPT handler registered\n");
    printk(KERN_INFO "MY_MODULE module loaded\n");
    return 0;
}

static void __exit my_tasklet_exit(void) {
    tasklet_kill(&my_tasklet);
    free_irq(IRQ, &my_dev_id);
    printk(KERN_INFO "MY_INTERRUPT unloaded\n");
    printk(KERN_INFO "MY_MODULE module unloaded\n");
}

module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
