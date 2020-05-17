#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");

#define IRQ 1

static int my_dev_id;
static int counter = 0;
static struct workqueue_struct *wq;

void my_workqueue_function(struct work_struct *work) {
    printk(KERN_INFO "MY_WORKQUEUE: working, counter: %d\n", ++counter);
}

DECLARE_WORK(workname, my_workqueue_function);

static irqreturn_t my_interrupt(int irq, void *dev) {
    printk(KERN_INFO "MY_INTERRUPT: starting\n");
    queue_work(wq, &workname);
    return IRQ_NONE;
}

static int __init my_init(void) {
    int ret = request_irq(IRQ, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id);
    if (ret) {
        printk(KERN_ERR "MY_MODULE: couldn't register handler\n");
        return ret;
    }

    wq = create_workqueue("my_workqueue");
    if (!wq) {
        free_irq(IRQ, &my_dev_id);
        printk(KERN_INFO "MY_MODULE: couldn't create workqueue\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "MY_MODULE: module loaded\n");
    return 0;
}

static void __exit my_exit(void) {
    flush_workqueue(wq);
    destroy_workqueue(wq);
    free_irq(IRQ, &my_dev_id);
    printk(KERN_INFO "MY_MODULE: module unloaded\n");
}

module_init(my_init)
module_exit(my_exit)
