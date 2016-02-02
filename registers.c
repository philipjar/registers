/*	'Registers' kernel module
    Copyright (C) 2015 Philip Lindner
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static int __init reg_init(void);
static void __exit reg_exit(void);
int proc_read(struct file *file, char __user *buf, size_t size, loff_t *offset);

module_init(reg_init);
module_exit(reg_exit);

struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read
};

char *output_buffer;
int len;

uint32_t eax;
uint32_t ebx;
uint32_t ecx;
uint32_t edx;

static int __init reg_init(void) {
    if (!proc_create("registers", 0666, NULL, &proc_ops)) {
        printk(KERN_ERR "registers: can't register proc\n");
        return -EINVAL;
    }
	output_buffer = kmalloc(64*sizeof(char), GFP_KERNEL);
	if (!output_buffer) {
		printk(KERN_ERR "registers: can't allocate buffer");
		return -ENOMEM;
	}
	printk(KERN_INFO "registers: successfully loaded\n");
    return 0;
}

static void __exit reg_exit(void) {
	remove_proc_entry("registers", NULL);
	if (output_buffer)
		kfree(output_buffer);
	printk(KERN_INFO "registers: successfully unloaded\n");
}

/* For such a simple module, we don't have to
	use seq_file. Instead, we just use an int
	to determine if output was given already */

int circeled = 0;

int proc_read(struct file *file, char __user *buf, size_t size, loff_t *offset) {
	if (circeled) {
		circeled = 0;
		return 0;
	}
	circeled = 1;

	asm volatile (
		"	movl %%eax, %0; \
			movl %%ebx, %1; \
			movl %%ecx, %2; \
			movl %%edx, %3 \
		"

		: 	"=r" (eax),
			"=r" (ebx),
			"=r" (ecx),
			"=r" (edx)

		:: "%eax","%ebx", "%ecx", "%edx"
	);

	sprintf(output_buffer, "eax: %d\nebx: %d\necx: %d\nedx: %d\n\n",
		eax, ebx, ecx, edx);

    len = strlen(output_buffer);
	copy_to_user(buf, output_buffer, len);
	return len;
}
