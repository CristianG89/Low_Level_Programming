////////////////////////////////////////////////
// EXERCISE 3 - LINUX KERNEL MODUHLE, TDT4258 //
////////////////////////////////////////////////

/* Rules that the kernel modules must follow to work with the Linux kernel:
- Kernel module must implement an interface that Linux kernel knows, so it knows how the module should be used
- A kernel module cannot call other functions than those which are defined in the Linux kernel itself.
- A kernel module must always function correctly even if different processes are trying to use it at the same time (paralelism).
- Kernel module is event based, so they cannot have infinite loops. Instead, they can only have functions to be called by other programmes. */

#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include "efm32gg.h"
#include "driver-gamepad.h"

//////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////

// The following structure tells the kernel how to invoke the functions open/release/read/write the file (driver)
static const struct file_operations file_ops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
	.fasync = fasync_handler
};
static struct cdev char_dev;
static const char *NAME = "gamepad";
static struct fasync_struct *fasync_queue;
static dev_t dev_num;
static struct class *cl;
static void *gpio_pc_model;
static void *gpio_pc_din;
static void *gpio_pc_dout;
static void *gpio_extipsell;
static void *gpio_extifall;
static void *gpio_extirise;
static void *gpio_ien;
static void *gpio_ifc;
static volatile uint8_t input_buttons;


//////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////

// Allocate device number for the driver
static int init_device_num() {
	int i;
	if ((i = alloc_chrdev_region(&dev_num, 0, 1, NAME))) {
		printk(KERN_ALERT "alloc_chrdev_region returned %d\n", i);
		return -1;
	}
	return 0;
}

// Deallocate device number.
static void deinit_device_num() {
	unregister_chrdev_region(dev_num, 1);
}

// Allocate GPIO
static int init_gpio() {
	// Asking for access to GPIO registers
	request_mem_region((unsigned long) GPIO_PC_MODEL, 1, NAME);
	request_mem_region((unsigned long) GPIO_PC_DIN, 1, NAME);
	request_mem_region((unsigned long) GPIO_PC_DOUT, 1, NAME);
	request_mem_region((unsigned long) GPIO_EXTIPSELL, 1, NAME);
	request_mem_region((unsigned long) GPIO_EXTIFALL, 1, NAME);
	request_mem_region((unsigned long) GPIO_EXTIRISE, 1, NAME);
	request_mem_region((unsigned long) GPIO_IEN, 1, NAME);

	// Memory map the allocated I/O region
	gpio_pc_model = ioremap_nocache((unsigned long) GPIO_PC_MODEL, 1);
	gpio_pc_din = ioremap_nocache((unsigned long) GPIO_PC_DIN, 1);
	gpio_pc_dout = ioremap_nocache((unsigned long) GPIO_PC_DOUT, 1);
	gpio_extipsell = ioremap_nocache((unsigned long) GPIO_EXTIPSELL, 1);
	gpio_extifall = ioremap_nocache((unsigned long) GPIO_EXTIFALL, 1);
	gpio_extirise = ioremap_nocache((unsigned long) GPIO_EXTIRISE, 1);
	gpio_ien = ioremap_nocache((unsigned long) GPIO_IEN, 1);
	gpio_ifc = ioremap_nocache((unsigned long) GPIO_IFC, 1);

	// NOTE: Accessing registers as done in Ex2 is not the preferred way in Linux. Instead, use funcions ioread32() and iowrite32().
	// Set button pins to input
	iowrite32(0x33333333, gpio_pc_model);
	// Set internal pull-up
	iowrite32(0xFF, gpio_pc_dout);
	// Enable external interrupts for buttons
	iowrite32(0x22222222, gpio_extipsell);
	// Set interrupt triggers to both rising and falling edge
	iowrite32(0xFF, gpio_extifall);
	iowrite32(0xFF, gpio_extirise);

	return 0;
}

// Deallocate GPIO
static void deinit_gpio() {
	// Unmap virtual addresses
	iounmap(gpio_pc_model);
	iounmap(gpio_pc_din);
	iounmap(gpio_pc_dout);
	iounmap(gpio_extipsell);
	iounmap(gpio_extifall);
	iounmap(gpio_extirise);
	iounmap(gpio_ien);
	iounmap(gpio_ifc);

	// Release GPIO registers
	release_mem_region((unsigned long) GPIO_PC_MODEL, 1);
	release_mem_region((unsigned long) GPIO_PC_DIN, 1);
	release_mem_region((unsigned long) GPIO_PC_DOUT, 1);
	release_mem_region((unsigned long) GPIO_EXTIPSELL, 1);
	release_mem_region((unsigned long) GPIO_EXTIFALL, 1);
	release_mem_region((unsigned long) GPIO_EXTIRISE, 1);
	release_mem_region((unsigned long) GPIO_IEN, 1);
	release_mem_region((unsigned long) GPIO_IFC, 1);
}

// GPIO interrupt handler
static irqreturn_t gpio_interrupt_handler(int irq_num, void *dev_id, struct pt_regs *regs) {
	// Full solution with interrupts. Driver handles the HW button interrupt, copies data into devnode-mapped memory and generates a signal.

	// Clear all interrupt flags (this is the only handler anyways)
	iowrite32(0xFFFFFFFF, gpio_ifc);
	// Read button status
	input_buttons = ~ioread8(gpio_pc_din);
	// Send signal
	if (fasync_queue) {
		kill_fasync(&fasync_queue, SIGIO, POLL_IN);
	}
	return IRQ_HANDLED;
}

// Enable interrupts GPIO interrupts
static int init_gpio_interrupts() {
	// Set interrupt handlers and relate to some device ID (char_dev)
	request_irq(GPIO_EVEN_IRQ_NUM, (irq_handler_t) gpio_interrupt_handler, 0, NAME, &char_dev);
	request_irq(GPIO_ODD_IRQ_NUM, (irq_handler_t) gpio_interrupt_handler, 0, NAME, &char_dev);
	
	// Enable all GPIO interrupts
	iowrite32(0xFF, gpio_ien);
	return 0;
}

// Disable interrupts GPIO interrupts
static void deinit_gpio_interrupts() {
	// Unset interrupt handlers
	free_irq(GPIO_EVEN_IRQ_NUM, &char_dev);
	free_irq(GPIO_ODD_IRQ_NUM, &char_dev);
	
	// Disable all GPIO interrupts
	iowrite32(0x0, gpio_ien);
}

// Allocate character device
static int init_char_dev() {
	int i;
	
	// Initialize the charater device
	cdev_init(&char_dev, &file_ops);		//The cdev structure is now initialized with a pointer to the file operations structure.
	char_dev.owner = THIS_MODULE;
	
	// Pass the cdev structure to the kernel with function cdev_add()
	if ((i = cdev_add(&char_dev, dev_num, 1))) {
		printk(KERN_ALERT "cdev_add returned %d\n", i);
		return -1;
	}
	// To make the driver appear as a file in the /dev directory, functions class_create() and device_create() must be called
	if ((cl = class_create(THIS_MODULE, NAME)) == NULL) {
		printk(KERN_ALERT "cdev_add returned NULL\n");
		return -1;
	}
	if (device_create(cl, NULL, dev_num, NULL, NAME) == NULL) {
		printk(KERN_ALERT "device_create returned NULL\n");
		return -1;
	}

	return 0;
}

// Deallocate character device
static void deinit_char_dev() {
	device_destroy(cl, dev_num);
	class_destroy(cl);
	cdev_del(&char_dev);
}

// FAsync handler
static int fasync_handler(int fd, struct file *file_p, int mode) {
	return fasync_helper(fd, file_p, mode, &fasync_queue);
}

// Open device file (driver)
static int my_open(struct inode *inodep, struct file *filep) {
	return 0;
}

// Close device file (driver)
static int my_release(struct inode *inodep, struct file *filep) {
	return 0;
}

// Read from device file (driver)
static ssize_t my_read(struct file *filep, char __user *buff, size_t count, loff_t *offp) {
	copy_to_user(buff, (uint8_t*)&input_buttons, 1);
	return sizeof(input_buttons);
}

// Write to device file (driver)
static ssize_t my_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp) {
	// Accept and ignore all data
	return count;
}

// Kernel module is loaded into Kernel space. All initializations must be done here
static int __init my_init() {
	printk(KERN_INFO "Initializing char device driver...\n");

	if (init_device_num() != 0) {
		printk(KERN_ALERT "Failed to initialize device number.\n");
		return -1;
	}
	if (init_gpio() != 0) {
		printk(KERN_ALERT "Failed to initialize GPIO.\n");
		return -1;
	}
	if (init_char_dev() != 0) {
		printk(KERN_ALERT "Failed to initialize character device.\n");
		return -1;
	}
	if (init_gpio_interrupts() != 0) {
		printk(KERN_ALERT "Failed to initialize interupts.\n");
		return -1;
	}

	printk(KERN_INFO "Initialization succeeded.\n");
	return 0;
	
	// NOTE: Clocks are handled by the OS and should not be enabled by writing registers directly, so nothing to do here.
}

// Kernel module is removed from kernel space. Everything previously allocated in my_init must be deallocated here
static void __exit my_exit() {
	printk(KERN_INFO "Deinitialzing module ...\n");

	deinit_gpio_interrupts();
	deinit_char_dev();
	deinit_gpio();
	deinit_device_num();

	printk(KERN_INFO "Deinitialization succeeded.\n");
}

// Linux Kernel needs to know the name of the init and exit functions of the kernel module:
module_init(my_init);
module_exit(my_exit);

//Description parameters of the Kernel module
MODULE_DESCRIPTION("Gamepad driver");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");