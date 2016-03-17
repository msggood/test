#include <linux/init.h>
#include <linux/types.h>
#include <linux/input/mt.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/major.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/cdev.h>


static struct class *hello_class;

static int hello_open(struct inode *inode,struct file *file)
{
    printk("hello_open\n");
    return 0;    
}
static int hello_read(struct file *file, char __user *user_buf, size_t size, loff_t *ppos)
{
    unsigned char data[3]="100";
    printk("hello_read\n");
    copy_to_user(user_buf,&data,size);
    return 0;
}
static int hello_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    unsigned char hello_buf[4];
//    printk("kernel write\n");
    copy_from_user(hello_buf,buf,3);
    hello_buf[3]='\0';
    printk("kernel receive hello_buf=%s\n",hello_buf);
    return 0;
}

static long hello_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
        case 1:
            printk("1 args's %ld\n",arg);
            break;
        case 2:            
            printk("2 args's %ld\n",arg);
            break;
        default:
            printk("cmd:%x,arg=%ld",cmd,arg);        
    }
    
    return 0;
}

static const struct file_operations hello_fops =
{                   
    .owner = THIS_MODULE,
    .open  = hello_open,
    .read  = hello_read,
    .write = hello_write,
    .unlocked_ioctl = hello_ioctl,
};
static struct cdev *hello_cdev;
struct device *class_dev;

static int __init hello_init(void)
{

    dev_t hell_devid;
    alloc_chrdev_region(&hell_devid, 0, 32768, "hello");
#if 0
    hello_cdev = cdev_alloc();
    cdev_init(hello_cdev, &hello_fops);
    hello_cdev->owner = THIS_MODULE;
    cdev_add(hello_cdev, hell_devid, 1);  //
//#else    
    register_chrdev(231, "hello", &hello_fops);
#endif
    hello_class=class_create(THIS_MODULE, "hello");
    class_dev=device_create(hello_class, NULL, hell_devid, NULL, "hello");

    return 0;
}
static void __exit hello_exit(void)
{
    printk("hello_exit\n");    
//    unregister_chrdev(231, "hello");
//    cdev_del(hello_cdev);
    device_unregister(class_dev);
    class_destroy(hello_class);
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
