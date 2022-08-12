/*
Sysplay Doc Ex 3
Video RAM Driver

Utilises memory-mapped IO to read and write into the VRAM address space.
Virtual addresses are allocated to the device.
Pointers to these addresses should not be directly dereferenced;
the APIs ioread8() and iowrite8() are used instead.

*/

#include <linux/init.h>   // contains macros for module initialisation
#include <linux/module.h> // includes the module related data structures
#include <linux/version.h>// for version compatibility of the module
#include <linux/kernel.h> // kernelspace header
#include <linux/kdev_t.h> // for dev_t and related macros
#include <linux/fs.h>     // for character driver related support
#include <linux/device.h> // for struct device
#include <linux/cdev.h>   // for cdev functions
#include <linux/uaccess.h>// contains functions for kernel to access userspace memory safely
#include <asm/io.h>       // contains APIs to access the device address space

/* Global variables */

#define VRAM_BASE 0x000A0000  // Start of Video RAM device address space
#define VRAM_SIZE 0x00020000  // Size of the same
static void __iomem *vram; // Pointer to a virtual io memory address
static dev_t first; // The first device number <major,minor>
static struct cdev c_dev; // Stores the character device attributes
static struct class *cl; // Device Class

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
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  int i; // counter
  u8 byte; // 8-bit unsigned integer

  if (*off >= VRAM_SIZE) // if offset is outside VRAM region
  {
    return 0;
  }
  if (*off + len > VRAM_SIZE) // if input string is too long
  {
    len = VRAM_SIZE - *off; // truncate
  }
  for (i=0; i<len; i++)
  {
    byte = ioread8((u8 *)vram + *off + i); // Function to read (8-bit) from virtual address space
    if (_copy_to_user(buf + i, &byte, 1)) // copies into userspace buffer
      return -EFAULT; // error in userspace memory access
  }
  *off += len; // Update offset
  return len;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  int i; // counter
  u8 byte; // 8-bit unsigned integer

  if (*off >= VRAM_SIZE) // if offset is outside VRAM region
  {
    return 0;
  }
  if (*off + len > VRAM_SIZE) // if output string is too long
  {
    len = VRAM_SIZE - *off; // truncate
  }
  for (i=0; i<len; i++)
  {
    if (_copy_from_user(&byte, buf + i, 1)) // copies into userspace buffer
      return -EFAULT; // error in userspace memory access
    iowrite8(byte, (u8 *)vram + *off +i); // Function to write (8-bit) to virtual address space
  }
  *off += len; // Update offset
  return len;
}

// file_operations is a structure (defined in <linux/fs.h>)
// that contains the file operations allowed for the device.

static struct file_operations Vram_FileOps =
{
  .owner = THIS_MODULE,//macro from <linux/module.h>
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static int __init VramDriver_init(void) /* Constructor */
{
  int ret; // return value for errors
  struct device *dev_ret;// structure for device attributes

  printk(KERN_ALERT "Hello. Inside the %s function\n",__FUNCTION__);

  if ((vram = ioremap(VRAM_BASE, VRAM_SIZE)) == NULL) 
  // Allocates a suitable address space for accessing the device
  {
    printk(KERN_ERR "Mapping Video RAM failed\n");
    return -ENOMEM; // error: not enough memory available
  }

  if ((ret=alloc_chrdev_region(&first,0,1,"VRAM"))<0)
  // Above function registers the character device files with the kernel,
  // and allocates an unused major number to the new device files.
 
    return ret; //if failed to register

  if (IS_ERR(cl = class_create(THIS_MODULE, "VramDriver")))
  //creates device class in /sys directory.

  {//if failed
    unregister_chrdev_region(first, 1);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "VramDriver0")))
  //populates device class with device info, and registers it in /sys directory.

  {//if failed
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return PTR_ERR(dev_ret);
  }

  cdev_init(&c_dev, &Vram_FileOps); // initialise character device with the file operations

  if ((ret = cdev_add(&c_dev, first, 1)) < 0)// add character device to the system
  {//if failed
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return ret;
  }

  return 0; //success
}

static void __exit VramDriver_exit(void) /* Destructor */
{
  printk(KERN_ALERT "Goodbye. Inside the %s function\n",__FUNCTION__);
  
  cdev_del(&c_dev);// remove character device from kernel (opposite of cdev_add)
  device_destroy(cl,first);
  class_destroy(cl);
  unregister_chrdev_region(first,1); //Unregister the character device files from the kernel
  iounmap(vram); // Returns the allocated address space to the kernel
}

module_init(VramDriver_init);
module_exit(VramDriver_exit);
//Macros to specify the init and exit functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Video RAM Driver");
//Macros for module related information
