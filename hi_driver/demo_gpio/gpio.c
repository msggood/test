#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <asm/io.h>
#include <asm/system.h>

#define DEVICE_NAME "hi_gpio"
static struct timer_list timer;

static int gpio_read(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{
    //    err = copy_to_user(buff,&key,sizeof(key));
    return 0;
}



//static irqreturn_t irq_interrupt(int irq,void *dev_id)
//{   
////wake_up_interruptible(&gpio_waitq);
//    printk("irq_interrupt\n");
//    return IRQ_HANDLED; //IRQ_HANDLED=1
//}


static unsigned int gpio_poll(struct file *file,struct poll_table_struct *wait)
{
#if 0
    unsigned int mask = 0;
    poll_wait(file,&gpio_waitq,wait);
#endif
    return 0;
}

//IRQF_SHARED|IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING 

static int gpio_open(struct inode *inode,struct file *file)
{

    return 0;

}

static int gpio_close(struct inode *inode,struct file *file)
{   
    //    free_irq(irq,&irq);
    return 0;
}


static long gpio_ioctl(struct file *file, unsigned int cmd , unsigned long arg)
{
    return 0;
}

static struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .open      = gpio_open,
    .release = gpio_close,
    .unlocked_ioctl   = gpio_ioctl,
    .read = gpio_read,
    .poll = gpio_poll,/*用户程序使用select调用的时候才会用到poll*/

};
/*

 *misc混合设备注册和注销

*/

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,/*次设备号*/
    .name = DEVICE_NAME,/*设备名*/
    .fops = &dev_fops,/*设备文件操作结构体*/

};


static void icut_timeout(unsigned long data)
{
}

#define GPIO_BASE 0x20140000
#define MUX_BASE 0x200F0000

static void gpio_output(unsigned int gpio_group,unsigned int gpio_offset,unsigned int value)
{
    unsigned int reg_val;
    if(gpio_group==15)
        gpio_group += 3;
    reg_val=readl(IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x400));
    reg_val |=(0x01<<gpio_offset);
    writel((reg_val),IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x400));
    writel(((0x01&value) << gpio_offset),IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + (1 << (gpio_offset +  2))));
}

static void gpio_input(unsigned int gpio_group,unsigned int gpio_offset)
{
    unsigned reg_val;
    if(gpio_group==15)
        gpio_group += 3;
    reg_val=readl(IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x400));
    reg_val &= ~(0x01<<gpio_offset);
    writel((reg_val),IO_ADDRESS(GPIO_BASE + (gpio_group << 16) + 0x400));
}


static int __init gpio_init(void)
{
    unsigned int gpio_group, gpio_offset;
    int ret;

    writel(0x0,IO_ADDRESS(MUX_BASE + 0x0DC));//管脚复用设置为 GPIO0_2

    gpio_group = 0;
    gpio_offset = 2;
    gpio_output(gpio_group,gpio_offset,1);
    gpio_input(gpio_group,gpio_offset);

    ret = misc_register(&misc);
    if(0 != ret)
    {
        printk("register device failed！ !\n");
        return -1;
    }
    init_timer(&timer);
    timer.function= icut_timeout;
    mod_timer(&timer, jiffies + msecs_to_jiffies(3000));
    return 0;   
}

static void __exit gpio_exit(void)
{
    del_timer(&timer);
    misc_deregister(&misc);
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
