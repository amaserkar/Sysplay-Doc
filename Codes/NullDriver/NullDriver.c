/*
Sysplay Doc Ex 2
Our First Character Driver (Null Driver)
With File Operation Functions, and creation of a Device Class.
Utilises automatic device file creation.
*/

#include <linux/init.h>   // contains macros for module initialisation
#include <linux/module.h> // includes the module related data structures
#include <linux/version.h>// for version compatibility of the module
#include <linux/kernel.h> // kernelspace header
#include <linux/kdev_t.h> // for dev_t and related macros
#include <linux/fs.h>     // for character driver related support
#include <linux/device.h> // for struct device
#include <linux/cdev.h>   // for cdev functions

static dev_t first; // Global variable for the first device number <major,minor>
static struct cdev c_dev; // Global variable for the character device
static struct class *cl; //Global variable for the device class

// Following are the File Operation functions

static int my_open(struct inode *i, struct file *f)
{//parameters: inode, file pointer (inode contains attributes of the file)
  printk(KERN_INFO "Driver Function open()\n");
  return 0;
}
static int my_close(struct inode *i, struct file *f)
{//parameters: inode, file pointer
  printk(KERN_INFO "Driver Function close()\n");
  return 0;
}
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{//parameters: file pointer, buffer pointer, buffer size, offset pointer
  printk(KERN_INFO "Driver Function read()\n");
  return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{//parameters: file pointer, buffer pointer, buffer size, offset pointer
  printk(KERN_INFO "Driver Function write()\n");
  return len;
}

// file_operations is a structure (defined in <linux/fs.h>)
// that contains the file operations allowed for the device.
static struct file_operations FileOps =
{
  .owner = THIS_MODULE,//macro from <linux/module.h>
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static int __init NullDriver_init(void)// Constructor
{
  int ret; // return value for errors
  struct device *dev_ret;// structure for device attributes

  printk(KERN_ALERT "Hello. Inside the %s function\n",__FUNCTION__);
  
  if ((ret=alloc_chrdev_region(&first,0,1,"OFCD"))<0)
  // Above function registers the character device files with the kernel,
  // and allocates an unused major number to the new device files.
    return ret; //if failed to register
  
  if (IS_ERR(cl = class_create(THIS_MODULE, "NullDriver")))
  //creates device class in /sys directory.
  {//if failed
    unregister_chrdev_region(first, 1);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "NullDriver0")))
  //populates device class with device info, and registers it in /sys directory.
  {//if failed
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return PTR_ERR(dev_ret);
  }

  cdev_init(&c_dev, &FileOps); // initialise character device with the file operations

  if ((ret = cdev_add(&c_dev, first, 1)) < 0)// add character device to the system
  {//if failed
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return ret;
  }
  return 0; //success
}

static void __exit NullDriver_exit(void) //Destructor
{
  printk(KERN_ALERT "Goodbye. Inside the %s function\n",__FUNCTION__);
  
  cdev_del(&c_dev);// remove character device from kernel (opposite of cdev_add)
  device_destroy(cl,first);
  class_destroy(cl);
  unregister_chrdev_region(first,1); //Unregister the character device files from the kernel
}

module_init(NullDriver_init);
module_exit(NullDriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Our First Character Driver - Null Driver");
