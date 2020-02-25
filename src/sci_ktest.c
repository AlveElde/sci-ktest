#define DEBUG
#define pr_fmt(fmt) KBUILD_MODNAME ": fn: %s, ln: %d: " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "scilib.h"

#include "sci_ktest.h"

MODULE_DESCRIPTION("Testing facilities for SCILib");
MODULE_AUTHOR("Alve Elde");
MODULE_LICENSE("GPL");

#define SCIL_INIT_FLAGS 0
#define SCIL_EXIT_FLAGS 0

static int create_msq(struct msq_ctx* msq)
{
    sci_error_t err;
    pr_devel(STATUS_START);

    err = SCILCreateMsgQueue(&msq->msq, 
                                msq->localAdapterNo, 
                                msq->remoteNodeId, 
                                msq->lmsqId,
                                msq->rmsqId, 
                                msq->maxMsgCount, 
                                msq->maxMsgSize, 
                                msq->timeout, 
                                msq->flags);
    switch (err)
    {
    case SCI_ERR_ILLEGAL_PARAMETER:
        pr_devel("SCI_ERR_ILLEGAL_PARAMETER: " STATUS_FAIL);
        return -42;
    case SCI_ERR_NOSPC:
        pr_devel("SCI_ERR_NOSPC: " STATUS_FAIL);
        return -42;
    default:
        pr_devel(STATUS_COMPLETE);
        return 0;
    }
}

static void perform_test(void)
{
    int ret;
    struct msq_ctx msq;

    pr_devel(STATUS_START);

    memset(&msq, 0, sizeof(struct msq_ctx));
    msq.msq             = NULL;
    msq.localAdapterNo  = 0;
    msq.remoteNodeId    = 99;
    msq.lmsqId          = 1;
    msq.rmsqId          = 2;
    msq.maxMsgCount     = 16;
    msq.maxMsgSize      = 128;
    msq.timeout         = 10;
    msq.flags           = 0;
    ret = create_msq(&msq);
    if(ret) {
        goto create_msq_err;
    }


    SCILRemoveMsgQueue(&msq.msq, 0);

create_msq_err:
    pr_devel(STATUS_COMPLETE);
    return;
}

static int __init sci_ktest_init(void)
{
    sci_error_t ret;
    pr_devel(STATUS_START);
    
    ret = SCILInit(SCIL_INIT_FLAGS);
    if(ret != SCI_ERR_OK) {
        pr_devel(STATUS_FAIL);
        return 0;
    }

    perform_test();

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