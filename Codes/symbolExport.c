#include <linux/module.h> // includes the module related definitions and the EXPORT_SYMBOL macros
#include <linux/device.h> // for class related definitions

static struct class *p_class1; // pointer to class
static struct class *get_class1(void)
{
  return p_class1;
}

// Export the pointer and the function as global symbols
// For use throughout the kernel space.
EXPORT_SYMBOL(p_class1);
EXPORT_SYMBOL_GPL(get_class1);

static int __init symbolExport_init(void) // Constructor
{
  if (IS_ERR(p_class1 = class_create(THIS_MODULE, "class1")))
  // creates a class under the /sys directory.
    return PTR_ERR(p_class1);
  return 0;
}

static void __exit symbolExport_exit(void) // Destructor
{
  class_destroy(p_class1); // removes class from /sys
}

module_init(symbolExport_init);
module_exit(symbolExport_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Global Symbol Exporting Driver");
