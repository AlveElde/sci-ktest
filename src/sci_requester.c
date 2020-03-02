#include "pr_fmt.h"

#include <linux/string.h>
#include <linux/delay.h>

#include "sci_ktest.h"

int send_request(struct msg_ctx *msg)
{
    sci_error_t err;
    pr_devel(DIS_STATUS_START);

    err = SCILSendMsg(*(msg->msq),
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
        pr_devel("SCI_ERR_EWOULD_BLOCK: " DIS_STATUS_FAIL);
        return -42;
    case SCI_ERR_NOT_CONNECTED:
        pr_devel("SCI_ERR_NOT_CONNECTED: " DIS_STATUS_FAIL);
        return -42;
    default:
        pr_devel("Unknown error code: " DIS_STATUS_FAIL);
        return -42;
    }
}

int connect_msq(struct msq_ctx *msq, int retry_max)
{
    int try_count = 0;
    sci_error_t err;
    pr_devel(DIS_STATUS_START);
    
    while(try_count < retry_max) {
        err = SCILConnectMsgQueue(&(msq->msq), 
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
        case SCI_ERR_CONNECTION_REFUSED:
            pr_devel("SCI_ERR_CONNECTION_REFUSED: " DIS_STATUS_FAIL);
            return -42;
        case SCI_ERR_NO_SUCH_SEGMENT:
            pr_devel("SCI_ERR_NOSPC: " DIS_STATUS_FAIL);
            return -42;
        default:
            pr_devel("Sleeping and retrying.. %d", err);
        }

        try_count++;
        if(try_count < retry_max) {
            msleep(1000);
        }
    }

    pr_devel(DIS_STATUS_FAIL);
    return -42;

}

void test_requester(unsigned int local_adapter_no, 
                            unsigned int remote_node_id)
{
    int ret;
    unsigned int size_free;
    char message[256] = MSG;
    struct msq_ctx msq;
    struct msg_ctx msg;

    pr_devel(DIS_STATUS_START);

    /* Connect to a remote MSQ */
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
    ret = connect_msq(&msq, 10);
    if(ret) {
        goto connect_msq_err;
    }

    /* Send message to MSQ */
    memset(&msg, 0, sizeof(struct msg_ctx));
    msg.msq         = &msq.msq;
    msg.msg         = message;
    msg.size        = MSG_LEN * sizeof(char);
    msg.free        = &size_free;
    msg.flags       = 0;
    ret = send_request(&msg);
    if(ret) {
        goto send_request_err;
    }

    pr_info("Message sent to responder: %s", message);
    pr_info("Bytes free in buffer: %d", size_free);

send_request_err:
    SCILDisconnectMsgQueue(&msq.msq, 0);
    pr_devel("Disconnect remote MSQ: " DIS_STATUS_COMPLETE);

connect_msq_err:
    pr_devel(DIS_STATUS_COMPLETE);
    return;
}