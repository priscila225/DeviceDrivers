#include "linux/fs.h"
#include "linux/cdev.h"
#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/device.h"
#include "asm/uaccess.h"
#include "asm/io.h"
#include "linux/slab.h"

#define DEVICE_NAME     "gpio"

#define NUM_IOS        8

#define GPIO_UP        1
#define GPIO_DOWN      0

#define GPF_BASE    0xFB000050
#define GPFCON      GPF_BASE
#define GPFDAT      GPF_BASE + 4

#define CLEAR_PORT_MASK     0xFFFFFFFF
#define SET_WRITE_PORT_MASK 0x55555555

/* prototypes */
int gpio_open(struct inode *inode, struct file *file);
ssize_t gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos);
ssize_t gpio_write(struct file *file, const char *buf, size_t count, loff_t *ppos);
int gpio_release(struct inode *inode, struct file *file);

/* per io structure */
struct gpio_device {
    int number;    
    int status;
    struct cdev cdev;
} gpio_dev[NUM_IOS];

/* file operations structure */
static struct file_operations gpio_fops = {
    .owner      = THIS_MODULE,
    .open       = gpio_open,
    .release    = gpio_release,
    .read       = gpio_read,
    .write      = gpio_write,
};

/* gpio driver major number */
static dev_t gpio_dev_number;

/* initialize io port - GPF */
void initIoPort(void)
{
    void __iomem *base = (void __iomem *)GPFCON;
    
    u32 port = __raw_readl(base);
        
    port &= ~CLEAR_PORT_MASK;
    port |= SET_WRITE_PORT_MASK;
    
    __raw_writel(port, base);
}

/* change io status */
void changeIoStatus(int io_num, int status)
{
    void __iomem *base = (void __iomem *)GPFDAT;
    u32 mask, data;
    
    data = __raw_readl(base);
    

    printk("\nSeting port:%1d status to:%1d\n", io_num, status);


    mask = 0x01 << io_num;
    
    switch (status) {
        
        case GPIO_UP:
            mask = ~mask;
            data &= mask;
            break;
            
        case GPIO_DOWN:
            data |= mask;
            break;
    }
    
    __raw_writel(data, base);
}

struct class *gpio_class;

/* driver initialization */
int __init gpio_init(void)
{
    int ret, i;
        
    /* request device major number */
    if ((ret = alloc_chrdev_region(&gpio_dev_number, 0, NUM_IOS, DEVICE_NAME) < 0)) {
        printk(KERN_DEBUG "Error registering device!\n");
        return ret;
    }
    
    /* create /sys entry */
    gpio_class = class_create(THIS_MODULE, DEVICE_NAME);

    /* init gpio GPIO port */
    initIoPort();
    
    /* init each io device */
    for (i = 0; i < NUM_IOS; i++) {
        
        /* init io status */
        gpio_dev[i].number = i;
        gpio_dev[i].status = GPIO_UP;
    
        /* connect file operations to this device */
        cdev_init(&gpio_dev[i].cdev, &gpio_fops);
        gpio_dev[i].cdev.owner = THIS_MODULE;
    
        /* connect major/minor numbers */
        if ((ret = cdev_add(&gpio_dev[i].cdev, (gpio_dev_number + i), 1))) {
            printk(KERN_DEBUG "Error adding device!\n");
            return ret;
        }
        
        /* init io status */
        changeIoStatus(gpio_dev[i].number, GPIO_UP);

	/* send uevent to udev to create /dev node */
        device_create(gpio_class, NULL, MKDEV(MAJOR(gpio_dev_number), i), NULL, "gpio%d", i);

    }
    
    printk("GPIO driver initialized.\n");
    
    return 0;        
}

/* driver exit */
int __exit gpio_exit(void)
{

    int i;
 
    /* release major number */
    unregister_chrdev_region(gpio_dev_number, NUM_IOS);
 
    /* delete devices */
    for (i = 0; i < NUM_IOS; i++) {
        device_destroy(gpio_class, MKDEV(MAJOR(gpio_dev_number), i));
        cdev_del(&gpio_dev[i].cdev);
    }
 
    /* destroy class */
    class_destroy(gpio_class);

    printk("Exiting gpio driver.\n");
    return 0;
}

/* open io file */
int gpio_open(struct inode *inode, struct file *file)
{
    struct gpio_device *gpio_devp;
    
    /* get cdev struct */
    gpio_devp = container_of(inode->i_cdev, struct gpio_device, cdev);
    
    /* save cdev pointer */
    file->private_data = gpio_devp;
    
    /* return OK */
    return 0;
}

/* close io file */
int gpio_release(struct inode *inode, struct file *file)
{
    /* return OK */
    return 0;
}

/* read io status */
ssize_t gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    struct gpio_device *gpio_devp = file->private_data;
    
    if (gpio_devp->status == GPIO_UP) {
        if (copy_to_user(buf, "1", 1))
            return -EIO;
    }
    else {
        if (copy_to_user(buf, "0", 1))
            return -EIO;
    }
    
    return 1;
}

ssize_t gpio_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    struct gpio_device *gpio_devp = file->private_data;
    char kbuf = 0;
    
    if (copy_from_user(&kbuf, buf, 1)) {
        return -EFAULT;
    }
    
    if (kbuf == '1') {
        changeIoStatus(gpio_devp->number, GPIO_UP);
        gpio_devp->status = GPIO_UP;
    }
    else if (kbuf == '0') {
        changeIoStatus(gpio_devp->number, GPIO_DOWN);
        gpio_devp->status = GPIO_DOWN;
    }
        
    return count;
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
