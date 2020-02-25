#define pr_fmt(fmt) KBUILD_MODNAME ": fn: %s, ln: %d: " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "sci_ktest.h"

MODULE_DESCRIPTION("Testing facilities for SCILib");
MODULE_AUTHOR("Alve Elde");
MODULE_LICENSE("GPL");

static unsigned int local_adapter_no    = 99;
static unsigned int remote_node_id      = 99;
static bool is_responder                = true;

module_param(local_adapter_no, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(remote_node_id, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(is_responder, bool, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

#define SCIL_INIT_FLAGS 0
#define SCIL_EXIT_FLAGS 0

static int __init sci_ktest_init(void)
{
    sci_error_t ret;
    pr_devel(STATUS_START);
    
    ret = SCILInit(SCIL_INIT_FLAGS);
    if(ret != SCI_ERR_OK) {
        pr_devel(STATUS_FAIL);
        return 0;
    }

    if(is_responder) {
        test_responder(local_adapter_no, remote_node_id);
    } else {
        test_requester(local_adapter_no, remote_node_id);
    }

    pr_devel(STATUS_COMPLETE);
    return 0;
}

static void __exit sci_ktest_exit(void)
{
    sci_error_t ret;
    pr_devel(STATUS_START);
    
    ret = SCILDestroy(SCIL_EXIT_FLAGS);
    if(ret != SCI_ERR_OK) {
        pr_devel(STATUS_FAIL);
        return;
    }

    pr_devel(STATUS_COMPLETE);
}

module_init(sci_ktest_init);
module_exit(sci_ktest_exit);