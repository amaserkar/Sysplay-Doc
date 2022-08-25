#include <linux/module.h> // for module-related definitions, including moduleparam.h
#include <linux/device.h>

static int value = 3; // Value to be changed using module parameters
module_param(value, int, 0764); // Set the name, type, and permissions of parameter
// Permission 0764 gives rwx to the user, rw- to the group, and r– for the others
// These permissions are for the file /sys/module/moduleParam/parameters/value

static int __init moduleParam_init(void) // Constructor
{
    printk(KERN_INFO "Loaded with Parameter.\nvalue = %d\n",value);
    return 0;
}

static void __exit moduleParam_exit(void) // Destructor
{
    printk(KERN_INFO "Unloaded.\nvalue = %d\n",value);
}

module_init(moduleParam_init);
module_exit(moduleParam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Module Parameter Demonstration Driver");
