/*
*****************************************************************
* Copyright 2015-2016, Shenzhen NiuTu All Rights Reserved
*
* ir204.c:
*    This file is use for receiving IrDA event.
* 
*
* @History
* Fly.Han 2016-07-19 am draft
*
*****************************************************************
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
//#include "hi_adc.h"

/***************************Define Macro*************************/
#define ADC_IRQ            48
#define SUPPORT_CHANNEL    0x1

#define SAR_ADC_BASE       0x200B0000
#define PERI_CRG32_BASE    0x2003007C

#define ADC_CTRL           0x00  //寄存器配置
#define ADC_GLITCH_SAMPEL  0x04  //滤波毛刺配置
#define ADC_TIME_SCAN      0x08
#define ADC_DATA_MASK      0x0c
#define ADC_INT_MASK       0x10  //int enable, 0:disable, 1:enable
#define ADC_INT_STATUS     0x14  //int state
#define ADC_INT_CLR        0x18  //int clr
#define ADC_START          0x1c  //start auto scan
#define ADC_STOP           0x20  //stop

#define ADC_IRQ_ENABLE     0x01
#define ADC_IRQ_DISABLE    0x00
//#define ADC_CLK_ENABLE     (1<<2)  //0为使能
#define ADC_CLK_ENABLE     (1<<1)  //0为使能

#define ADC_POWER_ENABLE   0x00
#define TIME_OUT           HZ * 5    // 队列等待时间 

//ADC_CTRL
#define ADC_ACTIVE_BIT(x)     (x << 24)    //precision, 11111100(6 bits),11111000(5 bits)...
#define DEGLITCH_BYPASS       (1 << 17)    //滤毛刺, 0:enable, 1:bypass
#define ADC_RESET             (1 << 15)    //reset, 0:quit, 1:enter reset
#define POWER_DOWN_MODE       (1 << 14)    //power_down, 0:disable, 1:enable
#define MODEL_SEL             (1 << 13)    //model, 0:single scan, 1:continuous scan
#define CHANNEL_SEL(x)        (1 << (x+8)) //channel, 0:A, 1:B, 2:C, 3:D
#define ADC_ZERO_MASK         0xffffff00
//ADC_DATA_MASK
//#define get_adc_data(x)       ((SAR_ADC_BASE+ADC_DATA_MASK) >> (x << 3) & 0xFF)
//ADC_INT_STATUS
//#define ADC_AUTO_BUSY         ((SAR_ADC_BASE+ADC_INT_STATUS) >> 4 & 0x01)
//#define ADC_INT_FLAG(x)       ((SAR_ADC_BASE+ADC_INT_STATUS) >> x & 0x01) //中断标志, 0:A, 1:B, 2:C, 3:D
//ADC_INT_CLR
//#define ADC_INT_CLR(x)        ((SAR_ADC_BASE+ADC_INT_CLR) | 0x01 << x) //清除中断标志, 0:A, 1:B, 2:C, 3:D

/***********************Define Globle Variate********************/
struct his_adc_driver{
    struct proc_dir_entry  *adc_file;
    void __iomem           *adc_reg_base;
    int                    flag;
    wait_queue_head_t      irq_wait;
};

static char *adc_proc_name = "sar_adc";
static struct his_adc_driver his_adc;

/****************Driver Information Indicate********************/
MODULE_AUTHOR("SZNiuTu.");
MODULE_DESCRIPTION("Sar_adc Driver");
MODULE_LICENSE("GPL");

int read_channel(int channel)
{
    unsigned int adc_ctrl_reg = 0, adc_data;

    // enable the interrupt
    writel(ADC_IRQ_ENABLE, his_adc.adc_reg_base + ADC_INT_MASK);

    // choose the channel
    adc_ctrl_reg = readl(his_adc.adc_reg_base + ADC_CTRL);
    adc_ctrl_reg |= CHANNEL_SEL(channel);
	printk("channel %d,adc_ctrl reg is %x\n",channel, adc_ctrl_reg);
    writel(adc_ctrl_reg, his_adc.adc_reg_base + ADC_CTRL);

    // start convert
    writel(0x01, his_adc.adc_reg_base + ADC_START);
#if 0
    wait_event_timeout(his_adc.irq_wait, his_adc.flag, TIME_OUT);
    if (0 == his_adc.flag)
        return -EFAULT;
    his_adc.flag = 0;
#endif
    // disable the interrupt
    writel(ADC_IRQ_DISABLE, his_adc.adc_reg_base + ADC_INT_MASK);

    // get the result
    adc_data = readl(his_adc.adc_reg_base + ADC_DATA_MASK);
	printk("adc data is %x\n", adc_data);
    return (adc_data >> (channel<<3) & 0xff);
}

static int hisi_adc_proc_read(char *page, char **start,
	off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i;
    for (i = 0; i < SUPPORT_CHANNEL; i++)
    {
        page[i] = read_channel(i);
        len += sizeof(page);
        udelay(1);
    }
    *eof = 1;
    return len;
}
	
#if 0
static irqreturn_t sar_adc_interrupt(int irq, void *id)
{
	int i,channel = 0;
	unsigned int int_flags,int_clrs;

	int_flags = readl(his_adc.adc_reg_base + ADC_INT_STATUS);
	printk("int flags is:%x\n", int_flags);
	for (i = 0; i < SUPPORT_CHANNEL; i++)
	{
		if(((int_flags >> i )& 0x1) > 0) channel = i;
		break;
	}
    // clear the interrupt
    int_clrs = readl(his_adc.adc_reg_base + ADC_INT_CLR);
	int_clrs |= 0x1 << channel;
    writel(int_clrs, his_adc.adc_reg_base + ADC_INT_CLR);
	//set the flag
    his_adc.flag = 1;
    wake_up(&his_adc.irq_wait);

    return 0;
}
#endif

int hisi_init_adc(void)
{
	int retval = 0;
	unsigned int adc_ctrl_reg=0,clk_reg;
	#define MUX_BASE 0x200F0000
	writel(0x0,IO_ADDRESS(MUX_BASE + 0x15C));
	
//	struct proc_dir_entry *parent;
//	parent = proc_mkdir ("adc", NULL);
//  his_adc.adc_file = create_proc_read_entry(adc_proc_name, 0744, parent,hisi_adc_proc_read, NULL);
    his_adc.adc_file = create_proc_read_entry(adc_proc_name, 0, NULL, hisi_adc_proc_read, NULL);
	if (his_adc.adc_file == NULL) {
		printk("%s: %s fail!\n", __func__, adc_proc_name);
		return -ENOMEM;
	}
	
#if 0
    retval = request_irq(ADC_IRQ, sar_adc_interrupt, 0, "SAR_ADC", NULL);
    if(0 != retval){
        printk("hi3518 ADC: failed to register IRQ(%d)\n", retval);
        goto ADC_INIT_FAIL1;
    }
#endif

    his_adc.adc_reg_base = ioremap_nocache((unsigned long)SAR_ADC_BASE, 0x20);
    if (NULL == his_adc.adc_reg_base){
        printk("function %s line %u failed\n", __FUNCTION__, __LINE__);
        retval = -EFAULT;
        goto ADC_INIT_FAIL2;
    }

	adc_ctrl_reg = readl(his_adc.adc_reg_base + ADC_CTRL);
	//adc reset
	adc_ctrl_reg |= ADC_RESET;
	writel(adc_ctrl_reg, his_adc.adc_reg_base + ADC_CTRL);
	adc_ctrl_reg &= ~ADC_RESET;
	writel(adc_ctrl_reg, his_adc.adc_reg_base + ADC_CTRL);
	//adc config
	adc_ctrl_reg |= ADC_ACTIVE_BIT(0xfc) & (~MODEL_SEL);
	//adc_ctrl_reg = adc_ctrl_reg&ADC_ZERO_MASK + 127;
	writel(adc_ctrl_reg, his_adc.adc_reg_base + ADC_CTRL);
	//CLK and cancell soft reset
	clk_reg = readl(IO_ADDRESS(PERI_CRG32_BASE));
//	clk_reg &= ~ADC_CLK_ENABLE;
	clk_reg |= ADC_CLK_ENABLE;
	writel(clk_reg, IO_ADDRESS(PERI_CRG32_BASE));
	
//    his_adc.flag = 0;
//    init_waitqueue_head(&his_adc.irq_wait);
    return 0;

ADC_INIT_FAIL2:
//    free_irq(ADC_IRQ, NULL);
//ADC_INIT_FAIL1:
    remove_proc_entry(adc_proc_name, NULL);
	return retval;
}
void hisi_exit_adc(void)
{
//    free_irq(ADC_IRQ, NULL);
    iounmap(his_adc.adc_reg_base);
    remove_proc_entry(adc_proc_name, NULL);
}

module_init(hisi_init_adc);
module_exit(hisi_exit_adc);

