#include "scilib.h"



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

int create_msq(struct msq_ctx *msq, int retry_max);
int connect_msq(struct msq_ctx *msq, int retry_max);
int send_request(struct msg_ctx *msg);
int receive_request(struct msg_ctx *msg, int retry_max);