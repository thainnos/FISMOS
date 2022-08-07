// SPDX-License-Identifier: GPL-2.0
/*
 * Linux kernel module, that allows utilization of the FISMOS.
 *
 * The values of the device can be accessed from user space on
 * /dev/fismos_****
 *
 * The device tree overlay needs to be loaded prior to inserting this module.
 */

/************************/
/*	Defines				*/
/************************/
// Uncomment, if more output to dmesg is needed!
#define DEBUG

#define ASCII_OFFSET 65
#define ASCII_OFFSET_REVERSE 48

#define AXI_SLAVE_ADDRESS_INSTRUCTION_REGISTER 0x43c00000
#define AXI_SLAVE_ADDRESS_STATUS_REGISTER 0x43c00004
#define AXI_SLAVE_ADDRESS_DATA_BUFFER_BASE 0x43c00008

#define MODULE_NAME "fismos"

/************************/
/*	Includes			*/
/************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/interrupt.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <asm/io.h>

/************************/
/*	Variables			*/
/************************/
static int *virtual_addr_instruction_register;
static phys_addr_t physical_addr_instruction_register;
static char instruction_register_input_to_LKM [10] = "000000000\n";

static int *virtual_addr_status_register;
static phys_addr_t physical_addr_status_register;

static int *virtual_addr_data_buffer;
static phys_addr_t physical_addr_data_buffer;
static char data_buffer_input_to_LKM [65] = 
	"0000000000000000000000000000000000000000000000000000000000000000\n";

DEFINE_MUTEX(mutex_instruction_register);
DEFINE_MUTEX(mutex_status_register);
DEFINE_MUTEX(mutex_data_buffer_in);
DEFINE_MUTEX(mutex_data_buffer_out);
DEFINE_MUTEX(mutex_data_buffer);

/****************************/
/*	DATA BUFFER				*/
/****************************/
/*
 * open: no functionality so far
 * @inode: Index Node
 * @filp: File Pointer
 *
 * RETURN: 0 on success
 */
static int data_buffer_open(struct inode *inode, struct file *filp)
{
	pr_devel("%s: %s called\n", MODULE_NAME, __func__);
	return 0; /* SUCCESS */
}

/*
 * data_buffer_read: copies state array to the user space
 * returns number of bytes that could be read by user space app
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t data_buffer_read(struct file *filp, char __user *user,
				size_t count, loff_t *offset)
{
	int ret = -1;
	int data_buffer_in = 0;
	char output[64];

	pr_devel("%s: read function called\n", MODULE_NAME);
	physical_addr_status_register = AXI_SLAVE_ADDRESS_DATA_BUFFER_BASE;
	virtual_addr_status_register = phys_to_virt(physical_addr_status_register);
	pr_devel("%s: Physical address:%llx\n",MODULE_NAME, 
		physical_addr_status_register);
	pr_devel("%s: Virtual address:%p\n",MODULE_NAME, 
		virtual_addr_status_register);

	mutex_lock(&mutex_data_buffer_out);
	virtual_addr_status_register = phys_to_virt(
				physical_addr_status_register);
		data_buffer_in = *virtual_addr_status_register;
		sprintf(output, "%x", data_buffer_in);
		output[(sizeof(output)-1)] = '\n';
		ret = simple_read_from_buffer(user, count, offset,
				output, sizeof(output));

	mutex_unlock(&mutex_data_buffer_out);
	pr_devel("%s: bytes read:%d\n",MODULE_NAME, ret);

	return ret;
}

/*
 * write: write access to misc device is not allowed
 * Note: Implementation is basically obsolete, as `mode=0444` is set
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t data_buffer_write(struct file *filp, const char __user *user,
				size_t count, loff_t *offset)
{
	int ret = -1;
	int err = -1;
	int bytes_written = 0;
	int data_buffer_write = 0;
	int *PTR_data_buffer_write = &data_buffer_write;
	char data_buffer_write_string[66] = "000000000\0";

	pr_devel("%s: %s start\n", MODULE_NAME, __func__);
	data_buffer_write = 0;

	//Here I am the only user of the ressource!
	mutex_lock(&mutex_data_buffer);
	ret = simple_write_to_buffer(data_buffer_input_to_LKM, 
		sizeof(data_buffer_input_to_LKM),offset, user, count);
	pr_devel("%s: bytes_written: %d\n", MODULE_NAME, bytes_written);

	strscpy(data_buffer_write_string, data_buffer_input_to_LKM, 
		sizeof(data_buffer_write_string));
	data_buffer_write_string[9] = '\0';

	pr_devel("%s: data_buffer_write_string: %s\n", MODULE_NAME, 
		data_buffer_write_string);
	err = kstrtoint(data_buffer_write_string, 16, PTR_data_buffer_write);
	pr_devel("%s: data_buffer_write: %d\n", MODULE_NAME, data_buffer_write);

	physical_addr_data_buffer = AXI_SLAVE_ADDRESS_DATA_BUFFER_BASE;
	virtual_addr_data_buffer = phys_to_virt(physical_addr_data_buffer);
	// *virtual_addr_data_buffer = data_buffer_write;
	mutex_unlock(&mutex_data_buffer);

	pr_devel("%s: %s finish\n", MODULE_NAME, __func__);

	return ret;
}

/* file operations */
static const struct file_operations fops_data_buffer = {
	.owner   = THIS_MODULE,
	.open    = data_buffer_open,
	.read    = data_buffer_read,
	.write	 = data_buffer_write,
	.llseek  = noop_llseek
};

/* information on fismos_data_buffer driver */
static struct miscdevice fismos_data_buffer = {
	.minor = MISC_DYNAMIC_MINOR, /* minor number will be assigned by kernel */
	.name = "fismos_data_buffer", /* device file /dev/hello will be created */
	.fops = &fops_data_buffer, /* pointer to file operations */
	.mode = 0666 /* Set wrights of driver*/
};


/****************************/
/*	STATUS REGISTER			*/
/****************************/

/*
 * open: no functionality so far
 * @inode: Index Node
 * @filp: File Pointer
 *
 * RETURN: 0 on success
 */
static int status_register_open(struct inode *inode, struct file *filp)
{
	pr_devel("%s: %s called\n", MODULE_NAME, __func__);
	return 0; /* SUCCESS */
}

/*
 * status_register_read: copies state array to the user space
 * returns number of bytes that could be read by user space app
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t status_register_read(struct file *filp, char __user *user,
				size_t count, loff_t *offset)
{
	int ret = -1;
	int status_register = 0;
	char output[9];

	pr_devel("%s: read function called\n", MODULE_NAME);
	physical_addr_status_register = AXI_SLAVE_ADDRESS_STATUS_REGISTER;
	virtual_addr_status_register = phys_to_virt(physical_addr_status_register);
	pr_devel("%s: Physical address:%llx\n",MODULE_NAME, 
		physical_addr_status_register);
	pr_devel("%s: Virtual address:%p\n",MODULE_NAME, 
		virtual_addr_status_register);

	mutex_lock(&mutex_status_register);
	pr_devel("%s: value at address:%d\n",MODULE_NAME, 
		*virtual_addr_status_register);

	// Define output array
	status_register = *virtual_addr_status_register;
	sprintf(output, "%x", status_register);
	output[8] = '\n';

	ret = simple_read_from_buffer(user, count, offset,
				output, sizeof(output));
	mutex_unlock(&mutex_status_register);

	return ret;
}

/*
 * write: write access to misc device is not allowed
 * Note: Implementation is basically obsolete, as `mode=0444` is set
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t status_register_write(struct file *filp, const char __user *user,
				size_t count, loff_t *offset)
{
	pr_devel("%s: %s start\n", MODULE_NAME, __func__);

	// No read access allowed
	pr_err("%s: write to status_register is not allowed!\n", MODULE_NAME);
	return -ECANCELED;
}

/* file operations */
static const struct file_operations fops_status_register = {
	.owner   = THIS_MODULE,
	.open    = status_register_open,
	.read    = status_register_read,
	.write	 = status_register_write,
	.llseek  = noop_llseek
};

/* information on fismos_status_register driver */
static struct miscdevice fismos_status_register = {
	.minor = MISC_DYNAMIC_MINOR, 
	.name = "fismos_status_register",
	.fops = &fops_status_register, /* pointer to file operations */
	.mode = 0444 /* Set wrights of driver*/
};

/****************************/
/*	INSTRUCTION REGISTER	*/
/****************************/
/*
 * open: no functionality so far
 * @inode: Index Node
 * @filp: File Pointer
 *
 * RETURN: 0 on success
 */
static int instruction_register_open(struct inode *inode, struct file *filp)
{
	pr_devel("%s: %s called\n", MODULE_NAME, __func__);
	return 0; /* SUCCESS */
}

/*
 * instruction_register_read: copies state array to the user space
 * returns number of bytes that could be read by user space app
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t instruction_register_read(struct file *filp, char __user *user,
				size_t count, loff_t *offset)
{

	int ret = -1;
	int instruction_register = 0;
	char output[9];

	pr_devel("%s: read function called\n", MODULE_NAME);
	physical_addr_status_register = AXI_SLAVE_ADDRESS_INSTRUCTION_REGISTER;
	virtual_addr_status_register = phys_to_virt(physical_addr_status_register);
	pr_devel("%s: Physical address:%llx\n",MODULE_NAME, 
		physical_addr_status_register);
	pr_devel("%s: Virtual address:%p\n",MODULE_NAME, 
		virtual_addr_status_register);

	mutex_lock(&mutex_instruction_register);
	pr_devel("%s: value at address:%d\n",MODULE_NAME, 
		*virtual_addr_status_register);

	// Define output array
	instruction_register = *virtual_addr_status_register;
	sprintf(output, "%x", instruction_register);
	output[8] = '\n';

	ret = simple_read_from_buffer(user, count, offset,
				output, sizeof(output));
	mutex_unlock(&mutex_instruction_register);

	return ret;
}

/*
 * write: write access to misc device is not allowed
 * Note: Implementation is basically obsolete, as `mode=0444` is set
 * @filp: File Pointer
 * @user: Pointer to buffer
 * @count: Number of bytes to read
 * @offset: Pointer Offset
 */
static ssize_t instruction_register_write(struct file *filp, 
				const char __user *user, size_t count, loff_t *offset)
{
	int ret = -1;
	int err = -1;
	int bytes_written = 0;
	int instruction_register_write = 0;
	int *PTR_instruction_register_write = &instruction_register_write;
	char instruction_register_write_string[10] = "000000000\0";

	pr_devel("%s: %s start\n", MODULE_NAME, __func__);
	instruction_register_write = 0;

	//Here I am the only user of the ressource!
	mutex_lock(&mutex_instruction_register);
	ret = simple_write_to_buffer(instruction_register_input_to_LKM, 
		sizeof(instruction_register_input_to_LKM),offset, user, count);
	pr_devel("%s: bytes_written: %d\n", MODULE_NAME, bytes_written);

	strscpy(instruction_register_write_string, 
		instruction_register_input_to_LKM, 
		sizeof(instruction_register_write_string));
	instruction_register_write_string[9] = '\0';

	pr_devel("%s: instruction_register_write_string: %s\n", MODULE_NAME, 
		instruction_register_write_string);
	err = kstrtoint(instruction_register_write_string, 16, 
		PTR_instruction_register_write);
	pr_devel("%s: instruction_register_write: %d\n", MODULE_NAME, 
		instruction_register_write);

	physical_addr_instruction_register = AXI_SLAVE_ADDRESS_INSTRUCTION_REGISTER;
	virtual_addr_instruction_register = 
		phys_to_virt(physical_addr_instruction_register);
	// *virtual_addr_instruction_register = instruction_register_write;
	mutex_unlock(&mutex_instruction_register);

	pr_devel("%s: %s finish\n", MODULE_NAME, __func__);

	return ret;
}

/* file operations */
static const struct file_operations fops_instruction_register = {
	.owner   = THIS_MODULE,
	.open    = instruction_register_open,
	.read    = instruction_register_read,
	.write	 = instruction_register_write,
	.llseek  = noop_llseek
};

/* information on fismos_instruction_register driver */
static struct miscdevice fismos_instruction_register = {
	.minor = MISC_DYNAMIC_MINOR, 
	.name = "fismos_instruction_register",
	.fops = &fops_instruction_register, /* pointer to file operations */
	.mode = 0666 /* Set wrights of driver*/
};


/************************/
/*	Device Driver		*/
/************************/
/*
 * probe function will be called, if there is an appropriate device available
 */
static int fismos_probe(struct platform_device *pdev)
{
	pr_devel("%s: %s start\n", MODULE_NAME, __func__);

	pr_devel("%s: %s finish\n", MODULE_NAME, __func__);
	return 0;

};

/*
 * fismos_remove: is called on removal of device
 * Frees pwm device
 */
static int fismos_remove(struct platform_device *pdev)
{
	pr_devel("%s: %s start\n", MODULE_NAME, __func__);

	pr_devel("%s: %s finish\n", MODULE_NAME, __func__);
	return 0;
};


/************************/
/*	DT Check			*/
/************************/

static const struct of_device_id fismos_dt_ids[] = {
	{
        .compatible = "xlnx,fismos-1.0"
	}, {}
};
MODULE_DEVICE_TABLE(of, fismos_dt_ids);

static struct platform_driver fismos_driver = {
	.probe = fismos_probe,
	.remove = fismos_remove,
	.driver = {
		.name = MODULE_NAME,
		.of_match_table = fismos_dt_ids,
	},
};

/************************/
/*	INIT & EXIT			*/
/************************/
/*
 * fismos_init: register device driver and misc device
 * returns zero on success, non-zero error code otherwise
 */
static int __init fismos_init(void)
{
	int err;

	pr_devel("%s: %s start\n", MODULE_NAME, __func__);

	/* Register platform device */
	pr_devel("%s: initialise fismos_driver platform driver\n", MODULE_NAME);
	err = platform_driver_register(&fismos_driver);
	pr_devel("%s: platform driver initialised\n", MODULE_NAME);
	if (err) {
		pr_err("%s: initialise fismos_driver platform failed\n", MODULE_NAME);
		goto free_platform_driver;
	}

	/* Register instruction register devices */
	pr_devel("%s: register fismos_instruction_register device\n", MODULE_NAME);
	err = misc_register(&fismos_instruction_register);
	pr_devel("%s: registered fismos_instruction_register device: %d\n", 
		MODULE_NAME, err);
	if (err) {
		pr_err("%s: register device failed\n", MODULE_NAME);
		goto out_err;
	}

	/* Register status register devices */
	err = misc_register(&fismos_status_register);
	pr_devel("%s: registered fismos_status_register device: %d\n", 
		MODULE_NAME, err);
	if (err) {
		pr_err("%s: register device failed\n", MODULE_NAME);
		goto fail_status_register_init;
	}
	pr_devel("%s: misc device registered\n", MODULE_NAME);

	/* Register data buffer devices */
	err = misc_register(&fismos_data_buffer);
	pr_devel("%s: registered fismos_data_buffer device: %d\n", 
		MODULE_NAME, err);
	if (err) {
		pr_err("%s: register device failed\n", MODULE_NAME);
		goto fail_data_buffer_init;
	}
	pr_devel("%s: misc device registered\n", MODULE_NAME);

	/* Success */
	if (pr_devel("%s: %s finish\n", MODULE_NAME, __func__))
		pr_info("%s: registered\n", MODULE_NAME);
	return 0;

fail_data_buffer_init:
	pr_devel("%s: deregister fismos_data_buffer device\n", MODULE_NAME);
	misc_deregister(&fismos_data_buffer);

fail_status_register_init:
	pr_devel("%s: deregister misc device\n", MODULE_NAME);
	misc_deregister(&fismos_instruction_register);

free_platform_driver:
	platform_driver_unregister(&fismos_driver);
	
out_err:
	return err;
}

/*
 * fismos_exit - will be called when module is removed
 */
static void __exit fismos_exit(void)
{
	pr_devel("%s: %s start\n", MODULE_NAME, __func__);


	/* Unregister driver */
	pr_devel("%s: initialise platform driver\n", MODULE_NAME);
	platform_driver_unregister(&fismos_driver);

	/* Unregister data buffer device */
	pr_devel("%s: deregister data buffer device\n", MODULE_NAME);
	misc_deregister(&fismos_data_buffer);

	/* Unregister status register device */
	pr_devel("%s: deregister status register device\n", MODULE_NAME);
	misc_deregister(&fismos_status_register);

	/* Unregister instruction register device */
	pr_devel("%s: deregister instruction register device\n", MODULE_NAME);
	misc_deregister(&fismos_instruction_register);

	if (!pr_devel("%s: %s finish\n", MODULE_NAME, __func__))
		pr_info("%s: removed\n", MODULE_NAME);
}

/*
 * macros for register and register module
 */
module_init(fismos_init);
module_exit(fismos_exit);

/*
 * informationen regarding the kernel module
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Philipp Schubaur");
MODULE_DESCRIPTION("Button toggles LED and writes button value back to user");
