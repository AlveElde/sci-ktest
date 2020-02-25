#include "scilib.h"

#define STATUS_START    "Started.\n"
#define STATUS_COMPLETE "Completed.\n"
#define STATUS_FAIL     "Failed.\n"

struct msq_ctx {
    sci_msq_queue_t msq;
    unsigned int    localAdapterNo; 
    unsigned int    remoteNodeId;   
    unsigned int    lmsqId;
    unsigned int    rmsqId;
    unsigned int    maxMsgCount;    
    unsigned int    maxMsgSize;  
    unsigned int    timeout;
    unsigned int    flags;
};

struct msg_ctx {
    sci_msq_queue_t *msq;
    void            *msg;    
    unsigned int    size;
    unsigned int    *free;
    unsigned int    flags;
};


void test_requester(unsigned int local_adapter_no, 
                            unsigned int remote_node_id);

void test_responder(unsigned int local_adapter_no, 
                            unsigned int remote_node_id);