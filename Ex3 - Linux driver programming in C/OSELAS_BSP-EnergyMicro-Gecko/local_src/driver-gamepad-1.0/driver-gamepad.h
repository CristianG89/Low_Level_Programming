#ifndef DRIVER_H	//Statement to be sure this header file cannot be called more than once
#define DRIVER_H


#include "efm32gg.h"	// List of useful memory mapped registers

/////////////////////////////////////////////////////////////////
// PROTOTYPE FUNCTIONS
/////////////////////////////////////////////////////////////////
// Module
static int __init my_init(void);			// Kernel module is loaded into Kernel space. All initializations must be done here
static void __exit my_exit(void);			// Kernel module is removed from kernel space. Everything previously allocated in my_init must be deallocated here

// Allocate and deallocate components
static int init_device_num(void);			// Allocate device number for the driver
static void deinit_device_num(void);		// Deallocate device number.
static int init_gpio(void);					// Allocate GPIO
static void deinit_gpio(void);				// Deallocate GPIO
static int init_gpio_interrupts(void);		// Enable interrupts GPIO interrupts
static void deinit_gpio_interrupts(void);	// Disable interrupts GPIO interrupts
static int init_char_dev(void);				// Allocate character device
static void deinit_char_dev(void);			// Deallocate character device

// Interrupt and fasync handlers
static irqreturn_t gpio_interrupt_handler(int, void *, struct pt_regs *);		// GPIO interrupt handler
static int fasync_handler(int, struct file *, int);								// FAsync handler

// File IO (driver) manipulation
static int my_open(struct inode *, struct file *);								// Open device file (driver)
static int my_release(struct inode *, struct file *);							// Close device file (driver)
static ssize_t my_read(struct file *, char __user *, size_t, loff_t *);			// Read from device file (driver)
static ssize_t my_write(struct file *, const char __user *, size_t, loff_t *);	// Write to device file (driver)


#endif //DRIVER_H