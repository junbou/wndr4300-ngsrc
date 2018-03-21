/*
 * wndr4300-usbled.c
 * (C) 2008  DeltaNetworks Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/list.h>

#include "wndr4300-usbled.h"

static struct proc_dir_entry *usbled_root;

typedef struct {
	struct list_head list;
	struct proc_dir_entry *parent;
	int nr;
	void *driver;
	usbled_config handler;
} usbled_proc_handler;

static ssize_t usbled_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t usbled_proc_write(struct file *file, const char *buf, size_t count, void *data);

static struct file_operations usbled_proc_fops = {
	.read = (ssize_t (*) (struct file *, char __user *, size_t, loff_t *))usbled_proc_read,
	.write = (ssize_t (*) (struct file *, const char __user *, size_t, loff_t *))usbled_proc_write
};

static ssize_t usbled_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
#ifdef LINUX_2_4
	struct inode *inode = file->f_dentry->d_inode;
	struct proc_dir_entry *dent = inode->u.generic_ip;
#else
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
#endif
	char *page;
	int len = 0;
	
	if ((page = kmalloc(USBLED_MAX_BUFSZ, GFP_KERNEL)) == NULL)
		return -ENOBUFS;
	
	if (dent->data != NULL) {
		usbled_proc_handler *handler = (usbled_proc_handler *) dent->data;
		if (handler->handler.read != NULL)
			len += handler->handler.read(handler->driver, page + len, handler->nr);
	}
	len += 1;

	if (*ppos < len) {
		len = min_t(int, len - *ppos, count);
		if (copy_to_user(buf, (page + *ppos), len)) {
			kfree(page);
			return -EFAULT;
		}
		*ppos += len;
	} else {
		len = 0;
	}

	kfree(page);
	return len;
}


static ssize_t usbled_proc_write(struct file *file, const char *buf, size_t count, void *data)
{
#ifdef LINUX_2_4
	struct inode *inode = file->f_dentry->d_inode;
	struct proc_dir_entry *dent = inode->u.generic_ip;
#else
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
#endif
	char *page;
	int ret = -EINVAL;

	if ((page = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}
	page[count] = 0;
	
	if (dent->data != NULL) {
		usbled_proc_handler *handler = (usbled_proc_handler *) dent->data;
		if (handler->handler.write != NULL) {
			if ((ret = handler->handler.write(handler->driver, page, handler->nr)) >= 0)
				ret = count;
		}
	}

	kfree(page);
	return ret;
}

static usbled_proc_handler *usbled_tmp;
extern void turn_on_usbled(void);
extern void turn_off_usbled(void);
extern int get_usbled_state(void);

static int handle_state_read(void *driver, char *buf, int nr)
{
	return sprintf(buf, "%d\n", get_usbled_state());
}

static int handle_state_write(void *driver, char *buf, int nr)
{
	if (buf[0] == '1')
		turn_on_usbled();

	if (buf[0] == '0')
		turn_off_usbled();
    
	return 0;
}

usbled_config global_usbled_handler = {"state", handle_state_read, handle_state_write};

static int __init usbled_init(void)
{
	struct proc_dir_entry *p;
	int mode;

	if ((usbled_root = proc_mkdir("usbled", NULL)) == NULL) {
		printk("%s: proc_mkdir usbled failed.\n", __FILE__);
		goto usb_root_fail;
	}

	usbled_tmp = 0;
	usbled_tmp = (usbled_proc_handler *) kmalloc(sizeof(usbled_proc_handler), GFP_KERNEL);
	if (!usbled_tmp)
		goto usbled_proc_handler_fail;

	memcpy(&usbled_tmp->handler, &global_usbled_handler, sizeof(usbled_config));

	mode = 0;
	if (global_usbled_handler.read != NULL) mode |= S_IRUSR;
	if (global_usbled_handler.write != NULL) mode |= S_IWUSR;

	if ((p = create_proc_entry(global_usbled_handler.name, mode, usbled_root)) != NULL) {
		p->data = (void *) usbled_tmp;
		p->proc_fops = &usbled_proc_fops;
	}
	
	return 0;

usbled_proc_handler_fail:
	remove_proc_entry("usbled", NULL);
usb_root_fail:
	return -ENODEV;
}

static void __exit usbled_exit(void)
{
	remove_proc_entry("state", usbled_root);
	if (usbled_tmp) kfree(usbled_tmp);
	remove_proc_entry("usbled", NULL);
}

MODULE_AUTHOR("Felix Fietkau <openwrt@nbd.name>");
MODULE_LICENSE("GPL");

module_init(usbled_init);
module_exit(usbled_exit);
