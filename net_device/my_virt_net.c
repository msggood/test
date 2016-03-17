#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/ip.h>


#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>

static struct net_device * test_ndev;
static int ntest_open(struct net_device *dev)
{
    printk("ntest_open\n");
    return 0;
}
static int ntest_stop(struct net_device *dev)
{
    printk("ntest_stop\n");
    return 0;
}

static void emulator_rx_data(struct sk_buff *skb,
						   struct net_device *dev)
{
    unsigned char *type;
    struct iphdr *ih;
    __be32 *saddr, *daddr,tmp;
    unsigned char tmp_dev_addr[ETH_ALEN];
    struct ethhdr *ethhdr;
    struct sk_buff *rx_skb;
    
//    printk("emulator_rx_data\n");
    ethhdr =(struct ethhdr*)skb->data;
    memcpy(tmp_dev_addr,ethhdr->h_dest,ETH_ALEN);
    memcpy(ethhdr->h_dest,ethhdr->h_source,ETH_ALEN);
    memcpy(ethhdr->h_source,tmp_dev_addr,ETH_ALEN);
//    printk("memcpy(h_source\n");

    ih=(struct iphdr*)(skb->data+sizeof(struct ethhdr));
    saddr =&(ih->saddr);
    daddr =&(ih->daddr);

    tmp=*saddr;
    *saddr=*daddr;
    *saddr=tmp;

    type = skb->data + sizeof(ethhdr)+sizeof(struct iphdr);
    *type=0;
    ih->check=0;
//    printk("ip_fast_csum\n");
//    ih->check=ip_fast_csum((unsigned char*)ih,ih->ihl);
//    printk("dev_alloc_skb\n");
    rx_skb=dev_alloc_skb(skb->len+2);
//    printk("skb_reserve\n");
    skb_reserve(skb,2);
//    printk("skb_put\n");
    memcpy(skb_put(rx_skb,skb->len),skb->data,skb->len);    
    rx_skb->dev=dev;
    rx_skb->protocol=eth_type_trans(rx_skb,dev);
    rx_skb->ip_summed=CHECKSUM_UNNECESSARY;
    dev->stats.rx_packets++;
    dev->stats.rx_bytes += skb->len;
//    printk("netif_rx(rx_skb);\n");
    netif_rx(rx_skb);
}

static netdev_tx_t	ntest_start_xmit(struct sk_buff *skb,
						   struct net_device *dev)
{
    static int count;
    printk("ntest_start_xmit %d\n",count++);

    netif_stop_queue(dev);
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    emulator_rx_data(skb,dev);
    dev_kfree_skb(skb);
    netif_wake_queue(dev);
    return NETDEV_TX_OK;
}

static void ntest_timeout(struct net_device *dev)
{
    printk("ntest_timeout\n");
}
static int ntest_set_mac_address(struct net_device *dev,
                                  void *addr_p)
{
	struct sockaddr *addr = addr_p;
    printk("ntest_set_mac_address"); 
	if (netif_running(dev))
		return -EBUSY;
	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	return 0;
}

static const struct net_device_ops test_netdev_ops = {
	.ndo_open		= ntest_open,
	.ndo_stop		= ntest_stop,
	.ndo_start_xmit		= ntest_start_xmit,
	.ndo_tx_timeout		= ntest_timeout,
//	.ndo_set_multicast_list	= test_hash_table,
//	.ndo_do_ioctl		= test_ioctl,
	.ndo_change_mtu		= eth_change_mtu,
//	.ndo_set_features	= test_set_features,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address	= ntest_set_mac_address,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= dm9000_poll_controller,
#endif
};


static int virt_net_init(void)
{
    printk("virt_net_init\n"); 
    test_ndev=alloc_etherdev(0);    
	test_ndev->netdev_ops	= &test_netdev_ops;
    test_ndev->dev_addr[0]=0x00;
    test_ndev->dev_addr[1]=0x00;
    test_ndev->dev_addr[2]=0xaa;
    test_ndev->dev_addr[3]=0xbb;
    test_ndev->dev_addr[4]=0xcc;
    test_ndev->dev_addr[5]=0xdd;    
    
    test_ndev->flags      |= IFF_NOARP;
    test_ndev->features   |= NETIF_F_NO_CSUM;
    
    register_netdev(test_ndev);
    return 0;
}
static void virt_net_exit(void)
{
    unregister_netdev(test_ndev);
    free_netdev(test_ndev);
}
module_init(virt_net_init);
module_exit(virt_net_exit);
MODULE_AUTHOR("msggood@163.com");
MODULE_DESCRIPTION("msggood DM9000 network driver");
MODULE_LICENSE("GPL");



