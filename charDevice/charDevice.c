#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAX_DEVICES         1U
#define BUFFER_SIZE         100U
static char BUFFER[BUFFER_SIZE] = "HellooWorld\n";
static struct cdev *myCharDev = NULL;
struct class *myCharDeviceClassPtr = NULL;
static struct device *myCharDevicePtr = NULL;
static dev_t myCharDevId = 0;

static int myOpen(struct inode *inode, struct file *filep)
{
    printk("Requesteing to open myCharDevice\n");
    return 0;
}

static ssize_t myRead(struct file *filep, char __user *buf,
			size_t count, loff_t *ppos)
{
    printk("Read requested for %lu bytes from file pos: %llu\n", count, *ppos);
    if(buf == NULL) return 0;

    if(BUFFER_SIZE < (count + *ppos)){
        count = BUFFER_SIZE - *ppos;
    }

    if(copy_to_user(buf, &BUFFER[*ppos], count)){
        return -EFAULT;
    }

    *ppos += count;
    printk("Successfully read %lu bytes\n", count);
    printk("Updated file pos %llu bytes\n", *ppos);

    return count;
}

static ssize_t myWrite(struct file *filep, const char __user *buf,
			size_t count, loff_t *ppos)
{
    if(buf == NULL) return 0;

    printk(KERN_WARNING "Requesteing to write %lu bytes from file pos: %llu", count, *ppos);

    if(BUFFER_SIZE < (count + *ppos)){
        printk(KERN_WARNING "Requested to write more bytes than the buffer can hold, truncating data to write");
        count = BUFFER_SIZE - *ppos;
    }
    if(count == 0){
        return -ENOMEM;
    }
    
    if(copy_from_user(&BUFFER[*ppos], buf, count)){
        return -EFAULT;
    }

    *ppos += count;
    printk("Successfully wrote %lu bytes\n", count);
    printk("Updated file pos %llu bytes\n", *ppos);

    return count;
}

static const struct file_operations myFops = {
	.owner		= THIS_MODULE,
	.open		= myOpen,
	.read		= myRead,
	.write		= myWrite,

};

int charDeviceInit(void)
{
    printk("loading a char device");
    char name[] = "myCharDevice";
    struct cdev *cdev = NULL;
    struct class *myCharDeviceClass = NULL;
    struct device *myCharDevice = NULL;

    dev_t charDev = 0;
    int result = 0;

    //create device number
    result = alloc_chrdev_region(&charDev, 0, MAX_DEVICES, name);
    if(result) return -1;

    cdev = cdev_alloc();
    if(!cdev) return -2;

    cdev->owner = THIS_MODULE;
    cdev->ops = &myFops;
    kobject_set_name(&cdev->kobj, "%s", name);

    result = cdev_add(cdev, charDev, MAX_DEVICES);
    if(result) return -3;

    myCharDevId = charDev;
    myCharDev = cdev;

    printk(KERN_DEBUG "Successfully initialized char device with major number: %d and minor number: %d", MAJOR(myCharDevId), MINOR(myCharDevId));

    myCharDeviceClass = class_create(THIS_MODULE, "char_class");

    if(!myCharDeviceClass) return -4;

    myCharDevice = device_create(myCharDeviceClass, NULL, charDev, NULL, "myCharDevice");
    if(!myCharDevice) return -5;

    myCharDeviceClassPtr = myCharDeviceClass;
    myCharDevicePtr = myCharDevice;

    printk(KERN_DEBUG "Successfully created myCharDevice");
    return 0;
}

void charDeviceExit(void)
{
    device_destroy(myCharDeviceClassPtr, myCharDevId);
    class_destroy(myCharDeviceClassPtr);
	cdev_del(myCharDev);
    unregister_chrdev_region(myCharDevId, MAX_DEVICES);
}

module_init(charDeviceInit);
module_exit(charDeviceExit);
MODULE_LICENSE("GPL");