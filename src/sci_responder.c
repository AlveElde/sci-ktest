#include "pr_fmt.h"

#include <linux/string.h>

#include "sci_ktest.h"

int receive_request(struct msg_ctx *msg, int retry_max)
{
    int i;
    sci_error_t err;
    pr_devel(DIS_STATUS_START);

    for(i = 0; i < retry_max; i++) {
        err = SCILReceiveMsg(*(msg->msq),
                            msg->msg,
                            msg->size,
                            msg->free,
                            msg->flags);
        switch (err)
        {
        case SCI_ERR_OK:
            pr_devel(DIS_STATUS_COMPLETE);
            return 0;
        case SCI_ERR_EWOULD_BLOCK:
            pr_devel("SCI_ERR_EWOULD_BLOCK");
            break;
        case SCI_ERR_NOT_CONNECTED:
            pr_devel("SCI_ERR_NOT_CONNECTED: " DIS_STATUS_FAIL);
            return -42;
        case SCI_ERR_ILLEGAL_PARAMETER:
            pr_devel("SCI_ERR_ILLEGAL_PARAMETER: " DIS_STATUS_FAIL);
            return -42;
        default:
            pr_devel("Unknown error code: " DIS_STATUS_FAIL);
            return -42;
        }

        if(i + 1 < retry_max) {
            msleep(1000);
        }
    }

    pr_devel(DIS_STATUS_FAIL);
    return -42;
}

int create_msq(struct msq_ctx *msq, int retry_max)
{
    int i = 0;
    sci_error_t err;
    pr_devel(DIS_STATUS_START);

    for(i = 0; i < retry_max; i++) {
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
            pr_devel(DIS_STATUS_COMPLETE);
            return 0;
        case SCI_ERR_ILLEGAL_PARAMETER:
            pr_devel("SCI_ERR_ILLEGAL_PARAMETER: " DIS_STATUS_FAIL);
            return -42;
        case SCI_ERR_NOSPC:
            pr_devel("SCI_ERR_NOSPC: " DIS_STATUS_FAIL);
            return -42;
        default:
            pr_devel("Sleeping and retrying.. %d", err);
        }

        if(i + 1 < retry_max) {
            msleep(1000);
        }
    }

    pr_devel(DIS_STATUS_FAIL);
    return -42;

}

void test_responder(unsigned int local_adapter_no, 
                            unsigned int remote_node_id)
{
    int ret;
    unsigned int size_left;
    char message[256];
    struct msq_ctx msq;
    struct msg_ctx msg;

    pr_devel(DIS_STATUS_START);

    /* Create a local MSQ */
    memset(&msq, 0, sizeof(struct msq_ctx));
    msq.msq               = NULL;
    msq.localAdapterNo    = local_adapter_no;
    msq.remoteNodeId      = remote_node_id;
    msq.lmsqId            = 444;
    msq.rmsqId            = 444;
    msq.maxMsgCount       = 16;
    msq.maxMsgSize        = 128;
    msq.timeout           = 1234;
    msq.flags             = 0;
    ret = create_msq(&msq, 10);
    if(ret) {
        goto create_msq_err;
    }

    /* Receive message from MSQ */
    memset(&msg, 0, sizeof(struct msg_ctx));
    msg.msq         = &msq.msq;
    msg.msg         = message;
    msg.size        = MSG_LEN * sizeof(char);
    msg.free        = &size_left;
    msg.flags       = 0;
    ret = receive_request(&msg, 10);
    if(ret) {
        goto receive_request_err;
    }

    pr_info("Message received from requester: %s", message);
    pr_info("Bytes left in buffer: %d", size_left);

receive_request_err:
    SCILRemoveMsgQueue(&msq.msq, 0);
    pr_devel("Remove local MSQ: "DIS_STATUS_COMPLETE);

create_msq_err:
    pr_devel(DIS_STATUS_COMPLETE);
    return;
}