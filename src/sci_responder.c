#define pr_fmt(fmt) KBUILD_MODNAME ": fn: %s, ln: %d: " fmt, __func__, __LINE__

#include <linux/string.h>

#include "sci_ktest.h"

int receive_request(struct msg_ctx *msg)
{
    sci_error_t err;
    pr_devel(STATUS_START);

    err = SCILSendMsg(*(msg->msq),
                        msg->msg,
                        msg->size,
                        msg->free,
                        msg->flags);
    switch (err)
    {
    case SCI_ERR_OK:
        pr_devel(STATUS_COMPLETE);
        return 0;
    case SCI_ERR_EWOULD_BLOCK:
        pr_devel("SCI_ERR_EWOULD_BLOCK: " STATUS_FAIL);
        return -42;
    case SCI_ERR_NOT_CONNECTED:
        pr_devel("SCI_ERR_NOT_CONNECTED: " STATUS_FAIL);
        return -42;
    case SCI_ERR_ILLEGAL_PARAMETER:
        pr_devel("SCI_ERR_ILLEGAL_PARAMETER: " STATUS_FAIL);
        return -42;
    default:
        pr_devel("Unknown error code: " STATUS_FAIL);
        return -42;
    }
}

int create_msq(struct msq_ctx *msq)
{
    sci_error_t err;
    pr_devel(STATUS_START);

    err = SCILCreateMsgQueue(&(msq->msq), 
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

void test_responder(unsigned int local_adapter_no, 
                            unsigned int remote_node_id)
{
    int ret;
    unsigned int size_left;
    char message[30];
    struct msq_ctx msq;
    struct msg_ctx msg;

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

    /* Receive message from MSQ */
    memset(&msg, 0, sizeof(struct msg_ctx));
    msg.msq         = &msq.msq;
    msg.msg         = &message;
    msg.size        = strlen(message);
    msg.free        = &size_left;
    msg.flags       = 0;
    ret = receive_request(&msg);
    if(ret) {
        goto receive_request_err;
    }

receive_request_err:
    SCILRemoveMsgQueue(&msq.msq, 0);
    pr_devel("Remove local MSQ: "STATUS_COMPLETE);

create_msq_err:
    pr_devel(STATUS_COMPLETE);
    return;
}