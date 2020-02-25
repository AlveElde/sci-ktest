#define DEBUG
#define pr_fmt(fmt) KBUILD_MODNAME ": fn: %s, ln: %d: " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "scilib.h"

#include "sci_ktest.h"

MODULE_DESCRIPTION("Testing facilities for SCILib");
MODULE_AUTHOR("Alve Elde");
MODULE_LICENSE("GPL");

static unsigned int local_adapter_no    = 99;
static unsigned int remote_node_id      = 99;
static bool is_server                   = true;

module_param(local_adapter_no, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(remote_node_id, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(is_server, bool, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

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
    case SCI_ERR_OK:
        pr_devel(STATUS_COMPLETE);
        return 0;
    case SCI_ERR_ILLEGAL_PARAMETER:
        pr_devel("SCI_ERR_ILLEGAL_PARAMETER: " STATUS_FAIL);
        return -42;
    case SCI_ERR_NOSPC:
        pr_devel("SCI_ERR_NOSPC: " STATUS_FAIL);
        return -42;
    default:
        pr_devel("Unknown error code: " STATUS_FAIL);
        return -42;
    }
}

static void test_server(void)
{
    int ret;
    struct msq_ctx msq;

    pr_devel(STATUS_START);

    /* Create a local MSQ */
    memset(&msq, 0, sizeof(struct msq_ctx));
    msq.msq               = NULL;
    msq.localAdapterNo    = local_adapter_no;
    msq.remoteNodeId      = remote_node_id;
    msq.lmsqId            = 1;
    msq.rmsqId            = 2;
    msq.maxMsgCount       = 16;
    msq.maxMsgSize        = 128;
    msq.timeout           = 10;
    msq.flags             = 0;
    ret = create_msq(&msq);
    if(ret) {
        goto create_msq_err;
    }

    SCILRemoveMsgQueue(&msq.msq, 0);
    pr_devel("Remove local MSQ: "STATUS_COMPLETE);

create_msq_err:
    pr_devel(STATUS_COMPLETE);
    return;
}

static int connect_msq(struct msq_ctx* msq)
{
    sci_error_t err;
    pr_devel(STATUS_START);

    err = SCILConnectMsgQueue(&msq->msq, 
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
    case SCI_ERR_OK:
        pr_devel(STATUS_COMPLETE);
        return 0;
    case SCI_ERR_CONNECTION_REFUSED:
        pr_devel("SCI_ERR_CONNECTION_REFUSED: " STATUS_FAIL);
        return -42;
    case SCI_ERR_NO_SUCH_SEGMENT:
        pr_devel("SCI_ERR_NOSPC: " STATUS_FAIL);
        return -42;
    default:
        pr_devel("Unknown error code: " STATUS_FAIL);
        return -42;
    }
}

static void test_client(void)
{
    int ret;
    struct msq_ctx msq;

    pr_devel(STATUS_START);

    /* Connect to a remote MSQ */
    memset(&msq, 0, sizeof(struct msq_ctx));
    msq.msq               = NULL;
    msq.localAdapterNo    = local_adapter_no;
    msq.remoteNodeId      = remote_node_id;
    msq.lmsqId            = 1;
    msq.rmsqId            = 2;
    msq.maxMsgCount       = 16;
    msq.maxMsgSize        = 128;
    msq.timeout           = 10;
    msq.flags             = 0;
    ret = connect_msq(&msq);
    if(ret) {
        goto connect_msq_err;
    }

    SCILDisconnectMsgQueue(&msq.msq, 0);
    pr_devel("Disconnect remote MSQ: " STATUS_COMPLETE);

connect_msq_err:
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

    if(is_server) {
        test_server();
    } else {
        test_client();
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