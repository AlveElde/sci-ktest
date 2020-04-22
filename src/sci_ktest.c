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
static bool         is_initiator       = true;

module_param(local_adapter_no, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(remote_node_id, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(is_initiator, bool, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

MODULE_PARM_DESC(local_adapter_no, "");
MODULE_PARM_DESC(remote_node_id, "");
MODULE_PARM_DESC(is_initiator, "");

#define SCIL_INIT_FLAGS 0
#define SCIL_EXIT_FLAGS 0
#define DIS_TIMEOUT_SEC 10
#define DIS_MSG_SIZE  256
// #define MSG_REQ "Hello There!\n"
// #define MSG_LEN strlen(MSG_REQ) + 1
// #define MSG_RES "General Kenobi!\n"
// #define MSG_LEN strlen(MSG_RES) + 1

void test_requester(void)
{
    int ret;
    unsigned int size_free, size_left;
    char message_in[DIS_MSG_SIZE];
    char message_out[DIS_MSG_SIZE] = "Hello There";
    struct msq_ctx msq_in, msq_out;
    struct msg_ctx msg_in, msg_out;

    pr_devel(DIS_STATUS_START);

    /* Connect to a remote MSQ */
    memset(&msq_out, 0, sizeof(struct msq_ctx));
    msq_out.msq               = NULL;
    msq_out.localAdapterNo    = local_adapter_no;
    msq_out.remoteNodeId      = remote_node_id;
    msq_out.lmsqId            = 444;
    msq_out.rmsqId            = 444;
    msq_out.maxMsgCount       = 16;
    msq_out.maxMsgSize        = 128;
    msq_out.timeout           = 1234;
    msq_out.flags             = 0;
    ret = connect_msq(&msq_out, DIS_TIMEOUT_SEC);
    if(ret) {
        goto connect_msq_err;
    }
    
    ssleep(5);

    /* Send message to MSQ */
    memset(&msg_out, 0, sizeof(struct msg_ctx));
    msg_out.msq         = &msq_out.msq;
    msg_out.msg         = message_out;
    msg_out.size        = DIS_MSG_SIZE * sizeof(char);
    msg_out.free        = &size_free;
    msg_out.flags       = 0;
    ret = send_request(&msg_out);
    if(ret) {
        goto send_request_err;
    }

    pr_info("Message sent to responder: %s", message_out);
    pr_info("Bytes free in buffer: %d", size_free);

    /* Create a local MSQ */
    memset(&msq_in, 0, sizeof(struct msq_ctx));
    msq_in.msq               = NULL;
    msq_in.localAdapterNo    = local_adapter_no;
    msq_in.remoteNodeId      = remote_node_id;
    msq_in.lmsqId            = 445;
    msq_in.rmsqId            = 445;
    msq_in.maxMsgCount       = 16;
    msq_in.maxMsgSize        = 128;
    msq_in.timeout           = 1234;
    msq_in.flags             = 0;
    ret = create_msq(&msq_in, DIS_TIMEOUT_SEC);
    if(ret) {
        goto create_msq_err;
    }

    /* Receive message from MSQ */
    memset(&msg_in, 0, sizeof(struct msg_ctx));
    msg_in.msq         = &msq_in.msq;
    msg_in.msg         = message_in;
    msg_in.size        = DIS_MSG_SIZE * sizeof(char);
    msg_in.free        = &size_left;
    msg_in.flags       = 0;
    ret = receive_request(&msg_in, DIS_TIMEOUT_SEC);
    if(ret) {
        goto receive_request_err;
    }

    pr_info("Message received from responder: %s", message_in);
    pr_info("Bytes left in buffer: %d", size_left);

receive_request_err:
    SCILRemoveMsgQueue(&msq_in.msq, 0);
    pr_devel("Remove local MSQ: "DIS_STATUS_COMPLETE);

create_msq_err:
    pr_devel(DIS_STATUS_COMPLETE);

send_request_err:
    SCILDisconnectMsgQueue(&msq_out.msq, 0);
    pr_devel("Disconnect remote MSQ: " DIS_STATUS_COMPLETE);

connect_msq_err:
    pr_devel(DIS_STATUS_COMPLETE);
    return;
}

void test_responder(void)
{
    int ret;
    unsigned int size_free, size_left;
    char message_in[DIS_MSG_SIZE];
    char message_out[DIS_MSG_SIZE] = "General Kenobi\n";
    struct msq_ctx msq_in, msq_out;
    struct msg_ctx msg_in, msg_out;

    pr_devel(DIS_STATUS_START);

    /* Create a local MSQ */
    memset(&msq_in, 0, sizeof(struct msq_ctx));
    msq_in.msq               = NULL;
    msq_in.localAdapterNo    = local_adapter_no;
    msq_in.remoteNodeId      = remote_node_id;
    msq_in.lmsqId            = 444;
    msq_in.rmsqId            = 444;
    msq_in.maxMsgCount       = 16;
    msq_in.maxMsgSize        = 128;
    msq_in.timeout           = 1234;
    msq_in.flags             = 0;
    ret = create_msq(&msq_in, DIS_TIMEOUT_SEC);
    if(ret) {
        goto create_msq_err;
    }

    /* Receive message from MSQ */
    memset(&msg_in, 0, sizeof(struct msg_ctx));
    msg_in.msq         = &msq_in.msq;
    msg_in.msg         = message_in;
    msg_in.size        = DIS_MSG_SIZE * sizeof(char);
    msg_in.free        = &size_left;
    msg_in.flags       = 0;
    ret = receive_request(&msg_in, DIS_TIMEOUT_SEC);
    if(ret) {
        goto receive_request_err;
    }

    pr_info("Message received from requester: %s", message_in);
    pr_info("Bytes left in buffer: %d", size_left);

    /* Connect to a remote MSQ */
    memset(&msq_out, 0, sizeof(struct msq_ctx));
    msq_out.msq               = NULL;
    msq_out.localAdapterNo    = local_adapter_no;
    msq_out.remoteNodeId      = remote_node_id;
    msq_out.lmsqId            = 445;
    msq_out.rmsqId            = 445;
    msq_out.maxMsgCount       = 16;
    msq_out.maxMsgSize        = 128;
    msq_out.timeout           = 1234;
    msq_out.flags             = 0;
    ret = connect_msq(&msq_out, DIS_TIMEOUT_SEC);
    if(ret) {
        goto connect_msq_err;
    }
    
    msleep(5000);

    /* Send message to MSQ */
    memset(&msg_out, 0, sizeof(struct msg_ctx));
    msg_out.msq         = &msq_out.msq;
    msg_out.msg         = message_out;
    msg_out.size        = DIS_MSG_SIZE * sizeof(char);
    msg_out.free        = &size_free;
    msg_out.flags       = 0;
    ret = send_request(&msg_out);
    if(ret) {
        goto send_request_err;
    }

    pr_info("Message sent to requester: %s", message_out);
    pr_info("Bytes free in buffer: %d", size_free);

send_request_err:
    SCILDisconnectMsgQueue(&msq_out.msq, 0);
    pr_devel("Disconnect remote MSQ: " DIS_STATUS_COMPLETE);

connect_msq_err:
    pr_devel(DIS_STATUS_COMPLETE);

receive_request_err:
    SCILRemoveMsgQueue(&msq_in.msq, 0);
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

    if(is_initiator) {
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