#include <linux/init.h>
#include <linux/module.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");

static int __init init(void) {
    struct task_struct *task = &init_task;
    printk(KERN_INFO "MY_MODULE: Hello world!\n");

    do {
        printk(KERN_INFO "MY_MODULE: comm %s, pid %d, pcomm %s, ppid %d\n",
            task->comm, task->pid, task->parent->comm, task->parent->pid);
    } while((task = next_task(task)) != &init_task);

    printk(KERN_INFO "MY_MODULE: current: comm %s, pid %d, pcomm %s, ppid %d\n",
           current->comm, current->pid, current->parent->comm, current->parent->pid);

    return 0;
}

static void __exit exit(void) {
    printk(KERN_INFO "MY_MODULE: Good buy!\n");
}

module_init(init);
module_exit(exit);
