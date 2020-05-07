#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");

#define BUF_SIZE PAGE_SIZE

#define DIR_NAME "my_dir"
#define FILE_NAME "my_file"
#define SYMLINK_NAME "my_symlink"

static char *my_buf;
static char read_tmp_buf[BUF_SIZE];
static int read_index;
static int write_index;

static struct proc_dir_entry *dir = NULL;
static struct proc_dir_entry *file = NULL;
static struct proc_dir_entry *symlink = NULL;

ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos) {
    int free_space = (BUF_SIZE - write_index) + 1;

    if (count > free_space) {
        printk(KERN_ERR "overflow error\n");
        return -ENOSPC;
    }

    if (copy_from_user(&my_buf[write_index], buf, count)) {
        printk(KERN_ERR "copy_from_user error\n");
        return -EFAULT;
    }

    write_index += count;
    my_buf[write_index - 1] = 0;

    return count;
}

ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos) {
    int len;

    if (write_index == 0 || *f_pos > 0) {
        return 0;
    }

    if (read_index >= write_index) {
        read_index = 0;
    }

    len = snprintf(read_tmp_buf, BUF_SIZE, "%s\n", &my_buf[read_index]);
    if (copy_to_user(buf, read_tmp_buf, len)) {
        printk(KERN_ERR "copy_to_user error");
        return -EFAULT;
    }
    read_index += len;
    *f_pos += len;

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};


static void cleanup(void) {
    remove_proc_entry(FILE_NAME, dir);
    remove_proc_entry(DIR_NAME, NULL);
    remove_proc_entry(SYMLINK_NAME, NULL);
    vfree(my_buf);
    printk(KERN_INFO "module cleaned up");
}

static int __init init(void) {
    if (!(my_buf = vmalloc(BUF_SIZE))
        || !(dir = proc_mkdir(DIR_NAME, NULL))
        || !(file = proc_create(FILE_NAME, 0666, dir, &fops))
        || !(symlink = proc_symlink(SYMLINK_NAME, NULL, DIR_NAME "/" FILE_NAME))) {
        cleanup();
        printk(KERN_ERR "init error");
        return -ENOMEM;
    }

    memset(my_buf, 0, BUF_SIZE);
    read_index = 0;
    write_index = 0;

    printk(KERN_INFO "module loaded");
    return 0;
}

module_init(init);
module_exit(cleanup);
