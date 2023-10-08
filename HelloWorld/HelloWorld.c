#include <linux/module.h>
static int err_val = 0;

module_param(err_val, int, 0600);
MODULE_PARM_DESC(err_val, "Error value");
int init(void)
{
    if(err_val >= 0)
    {
        printk(KERN_NOTICE"HelloWorld is loaded %d", err_val);
        printk(KERN_DEBUG"HelloWorld is loaded %d", err_val);
    }
    else 
    {
        printk(KERN_ERR "HelloWorld failed to load %d", err_val);
        printk(KERN_EMERG "HelloWorld failed to load %d", err_val);
    }
    return err_val;
}

void remove(void)
{
    printk("HelloWorld is removed");
}

module_init(init);
module_exit(remove);
MODULE_LICENSE("GPL");