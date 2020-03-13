#include "pr_fmt.h"

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "sci_msq.h"

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

void test_requester(void)
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

void test_responder(void)
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

static int __init sci_ktest_init(void)
{
    sci_error_t ret;
    pr_devel(DIS_STATUS_START);
    
    ret = SCILInit(SCIL_INIT_FLAGS);
    if(ret != SCI_ERR_OK) {
        pr_devel(DIS_STATUS_FAIL);
        return 0;
    }

    if(is_responder) {
        test_responder();
    } else {
        test_requester();
    }

    pr_devel(DIS_STATUS_COMPLETE);
    return 0;
}

static void __exit sci_ktest_exit(void)
{
    sci_error_t ret;
    pr_devel(DIS_STATUS_START);
    
    ret = SCILDestroy(SCIL_EXIT_FLAGS);
    if(ret != SCI_ERR_OK) {
        pr_devel(DIS_STATUS_FAIL);
        return;
    }

    pr_devel(DIS_STATUS_COMPLETE);
}

module_init(sci_ktest_init);
module_exit(sci_ktest_exit);