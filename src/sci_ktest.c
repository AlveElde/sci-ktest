#define DEBUG
#define pr_fmt(fmt) KBUILD_MODNAME ": fn: %s, ln: %d: " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "sci_ktest.h"

MODULE_DESCRIPTION("Testing facilities for SCILib");
MODULE_AUTHOR("Alve Elde");
MODULE_LICENSE("GPL");


static int __init sci_ktest_init(void)
{
    pr_info(STATUS_START);
    pr_info(STATUS_COMPLETE);
    return 0;
}

static void __exit sci_ktest_exit(void)
{
    pr_info(STATUS_START);
    pr_info(STATUS_COMPLETE);
}

module_init(sci_ktest_init);
module_exit(sci_ktest_exit);