#ifndef SYMBOL_EXPORT_H
#define SYMBOL_EXPORT_H

#ifdef __KERNEL__ 
// Code within this block is accessible only to the kernel

#include <linux/device.h> // for class related definitions
extern struct class *p_class1; // pointer to class
extern struct class *get_class1(void); // function that returns pointer to class

#endif
#endif
