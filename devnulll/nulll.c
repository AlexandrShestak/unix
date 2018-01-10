#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandr Shestak <shestakalexandr@mail.ru>");
MODULE_DESCRIPTION("nulll");

static ssize_t nulll_write(struct file *, const char __user *, size_t, loff_t *);
static long nulll_ioctl(struct file *, unsigned int, unsigned long);

static unsigned long capacity = -1;
unsigned long current_size = 0;
struct mutex lock;
module_param(capacity, ulong, (S_IRUSR|S_IRGRP|S_IROTH));
MODULE_PARM_DESC(capacity, "Number of bytes that can be written to the module");


static struct file_operations nulll_fops = {
        .owner = THIS_MODULE,
        .unlocked_ioctl = nulll_ioctl,
        .write = nulll_write
};

static struct miscdevice nulll_misc_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "nulll",
        .fops = &nulll_fops
};

static ssize_t nulll_write(struct file *file, const char __user *out, size_t len, loff_t *off)
{
    ssize_t result;

    if (mutex_lock_interruptible(&lock)) {
        result = -ERESTARTSYS;
        goto out;
    }

    if (current_size == capacity) {
        result = -ENOSPC;
        goto out_unlock;
    } else if (capacity > 0 && current_size + len > capacity) {
        current_size += capacity - current_size;
        result = capacity - current_size;
        goto out_unlock;
    }

    current_size += len;
    result = len;

    out_unlock:
            mutex_unlock(&lock);

    out:
    return result;
}

static long nulll_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case BLKGETSIZE64:
            return put_user(current_size, (u64 __user *)arg);
    }
    return 0;
}

static int __init nulll_init(void)
{
    if (capacity < 0)
        return -1;
    misc_register(&nulll_misc_device);
    mutex_init(&lock);
    return 0;
}

static void __exit nulll_exit(void)
{
    misc_deregister(&nulll_misc_device);
    mutex_destroy(&lock);
}

module_init(nulll_init);
module_exit(nulll_exit);