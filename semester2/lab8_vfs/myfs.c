#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kotov");
MODULE_DESCRIPTION("myfs");

#define SLABNAME "my_cache"
#define MYFS_MAGIC_NUMBER 0xDEADBEAF

static int number = 31;
static struct myfs_inode **line = NULL;
static int occupied = 0;
struct kmem_cache *cache = NULL;

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};

static void myfs_put_super(struct super_block *sb) {
    printk(KERN_DEBUG "MYFS super block destroyed!\n");
}

static struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

struct myfs_inode *get_inode_from_cache(void) {
    if (occupied == number) {
        return NULL;
    }
    line[occupied] = kmem_cache_alloc(cache, GFP_KERNEL);
    return line[occupied++];
}

static struct inode *myfs_make_inode(struct super_block *sb, int mode) {
    struct inode *ret = new_inode(sb);
    struct myfs_inode *my_inode = NULL;

    if (ret) {
        inode_init_owner(ret, NULL, mode);
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        my_inode = get_inode_from_cache();
        if (my_inode) {
            my_inode->i_mode = ret->i_mode;
            my_inode->i_ino = ret->i_ino;
        }
        ret->i_private = my_inode;
    }
    return ret;
}

static int myfs_fill_sb(struct super_block *sb, void *data, int silent) {
    struct inode *root = NULL;

    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC_NUMBER;
    sb->s_op = &myfs_super_ops;

    root = myfs_make_inode(sb, S_IFDIR | 0755);
    if (!root) {
        printk(KERN_ERR "MYFS inode allocation failed!\n");
        return -ENOMEM;
    }
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    sb->s_root = d_make_root(root);
    if (!sb->s_root) {
        printk(KERN_ERR "MYFS root creation failed!\n");
        iput(root);
        return -ENOMEM;
    }

    return 0;
}

static struct dentry *myfs_mount(struct file_system_type *type, int flags, char const *dev, void *data) {
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);
    if (IS_ERR(entry))
        printk(KERN_ERR "MYFS mounting failed!\n");
    else
        printk(KERN_DEBUG "MYFS mounted!\n");
    return entry;
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "myfs",
    .mount = myfs_mount,
    .kill_sb = kill_anon_super,
};

static int __init myfs_init(void) {
    int i, ret;

    line = kmalloc(sizeof(void *) * number, GFP_KERNEL);
    if (!line) {
        printk(KERN_ERR "MYFS kmalloc error\n" );
        return -ENOMEM;
    }

    cache = kmem_cache_create(SLABNAME, sizeof(struct myfs_inode), 0, SLAB_POISON, NULL);
    if (!cache) {
        printk(KERN_ERR "MYFS kmem_cache_create error\n" );
        kfree(line);
        return -ENOMEM;
    }

    ret = register_filesystem(&myfs_type);
    if (ret != 0) {
        printk(KERN_ERR "MYFS_MODULE cannot register filesystem!\n");
        kmem_cache_destroy(cache);
        return ret;
    }
    printk(KERN_DEBUG "MYFS_MODULE loaded!\n");
    return 0;
}

static void __exit myfs_exit(void) {
    int i, ret;

    for (i = 0; i < occupied; i++) {
        kmem_cache_free(cache, line[i]);
    }
    kmem_cache_destroy(cache);
    kfree(line);

    ret = unregister_filesystem(&myfs_type);
    if (ret != 0) {
        printk(KERN_ERR "MYFS_MODULE cannot unregister filesystem!\n");
    }

    printk(KERN_DEBUG "MYFS_MODULE unloaded!\n");
}

module_init(myfs_init)
module_exit(myfs_exit)
