/*
Sysplay Doc Ex 1
Our First Driver
*/

#include <linux/init.h> // contains macros for module initialisation
#include <linux/module.h> // includes the module related data structures

int __init driver1_init(void)// Constructor - initialisation
{ // called during loading of module
  printk(KERN_ALERT "Hello. Inside the %s function\n",__FUNCTION__);
  //printk prints to /var/log/syslog
  return 0;
}

void driver1_exit(void) //Destructor - clean up
{ // called during unloading
  printk(KERN_ALERT "Goodbye. Inside the %s function\n",__FUNCTION__);
  //KERN_ALERT is a "log level" which specifies the importance of the message.
}

module_init(driver1_init);
module_exit(driver1_exit);
//Macros to specify the init and exit functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
//Macros for module related information
