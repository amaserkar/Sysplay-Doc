/* Sysplay Doc Ex 4
 * 
 * Userspace Application query_app.c to query variables from
 * Device Driver query_ioctl.c (kernelspace to userspace)
 */

#include <linux/init.h>   // contains macros for module initialisation
#include <linux/module.h> // includes the module related data structures
#include <linux/version.h>// for version compatibility of the module
#include <linux/kernel.h> // kernelspace header
#include <linux/fs.h>     // for character driver related support
#include <linux/device.h> // for struct device
#include <linux/cdev.h>   // for cdev functions
#include <linux/kdev_t.h> // for dev_t and related macros
#include <linux/uaccess.h>// contains functions for kernel to access userspace memory safely
#include <linux/errno.h>  // for int errno, which is set by system calls

#include "query_ioctl.h" // Header file with type definitions

/* Global variables */

#define FIRST_MINOR 0 // Minor number of character driver
#define MINOR_CNT 1 // No. of minor numbers

static dev_t dev; // The device number <major,minor>
static struct cdev c_dev; // Stores the character device attributes
static struct class *cl; // Device Class
static int status = 1, dignity = 3, ego = 5; // Variables to be queried

/* Device File Operations */

static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver Function open()\n");
  return 0;
}
static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver Function close()\n");
  return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
// compatible with kernel version 2.6.35 and above
{
  query_arg_t q; // structure for querying driver variables

  switch (cmd) // Call the required query function
  {
    case QUERY_GET_VARIABLES: // Function to read the variables from the driver
      q.status = status;
      q.dignity = dignity;
      q.ego = ego;
      if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t))) // copies into userspace (structure passed as arg)
        return -EACCES; // Error: Permission denied
      break;
    case QUERY_SET_VARIABLES: // Function to write the variables to the driver
      if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t))) // copies from userspace (structure passed as arg)
        return -EACCES; // Error: Permission denied
      status = q.status;
      dignity = q.dignity;
      ego = q.ego;
      break;
    case QUERY_CLR_VARIABLES: // Function to clear the variables in the driver
      status = 0;
      dignity = 0;
      ego = 0;
      break;
    default:
      return -EINVAL; // Error: invalid argument
  }
  return 0;
}

// file_operations is a structure (defined in <linux/fs.h>)
// that contains the file operations allowed for the device.

static struct file_operations query_FileOps =
{
  .owner = THIS_MODULE, //macro from <linux/module.h>
  .open = my_open,
  .release = my_close,
  .unlocked_ioctl = my_ioctl // compatible with kernel version 2.6.35 and above
};

static int __init query_ioctl_init(void) /* Constructor */
{
  int ret; // return value for errors
  struct device *dev_ret;// structure for device attributes

  printk(KERN_ALERT "Hello. Inside the %s function\n",__FUNCTION__);

  if ((ret = alloc_chrdev_region(&dev,FIRST_MINOR,MINOR_CNT,"query_ioctl")) < 0)
  // Above function registers the character device file with the kernel,
  // and allocates an unused major number to the new device file. 
    return ret; //if failed to register

  if (IS_ERR(cl = class_create(THIS_MODULE, "QueryIoctl")))
  //creates device class in /sys directory.

  {//if failed
    unregister_chrdev_region(dev,MINOR_CNT);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "QueryIoctl0")))
  //populates device class with device info, and registers it in /sys directory.

  {//if failed
    class_destroy(cl);
    unregister_chrdev_region(dev,MINOR_CNT);
    return PTR_ERR(dev_ret);
  }

  cdev_init(&c_dev, &query_FileOps); // initialise character device with the file operations

  if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)// add character device to the system
  {//if failed
    device_destroy(cl,dev);
    class_destroy(cl);
    unregister_chrdev_region(dev,MINOR_CNT);
    return ret;
  }

  return 0; //success
}


static void __exit query_ioctl_exit(void) /* Destructor */
{
  printk(KERN_ALERT "Goodbye. Inside the %s function\n",__FUNCTION__);
  cdev_del(&c_dev); // remove character device from kernel (opposite of cdev_add)  
  device_destroy(cl, dev);
  class_destroy(cl);
  unregister_chrdev_region(dev, MINOR_CNT); //Unregister the character device file from the kernel
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);
//Macros to specify the init and exit functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
//Macros for module related information
