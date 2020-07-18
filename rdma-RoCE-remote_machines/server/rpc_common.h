#ifndef __RPC_COMMON_H__
#define __RPC_COMMON_H__

#define MAX_RECV_WQES 128
#define CLIENT_MAX_SEND_WQES MAX_RECV_WQES
#define CLIENT_MAX_RECV_WQES MAX_RECV_WQES
#define CLIENT_SEND_CQ_SIZE 10
#define CLIENT_RECV_CQ_SIZE 10
#define HOST_RECV_CQ_SIZE 10 
#define HOST_SEND_CQ_SIZE 10
#define CLIENT_TOTAL_DATA_TO_SERVER_SIZE 10
#define CLIENT_TOTAL_DATA_FROM_SERVER_SIZE 10
#define HOST_TOTAL_DATA_FROM_CLIENT_SIZE 10
#define HOST_TOTAL_DATA_TO_CLIENT_SIZE 10
#define IB_PORT_SERVER 1
#define IB_PORT_CLIENT 1

#define ARG_BUF_SIZE 10
#define RES_BUF_SIZE 10

#define TCP_PORT_OFFSET 23456
#define TCP_PORT_RANGE 1000


struct ib_info_t {
    int lid;
    int qpn;
    int mkey_args;
    long long addr_args;
    int mkey_result;
    long long addr_result;
    ibv_gid gid;
};
struct ib_resources_t {
	struct ibv_context *context;
	struct ibv_pd *pd;
	struct ibv_qp *qp;
	//struct ibv_cq *cq;
	struct ibv_cq *recv_cq;
	struct ibv_cq *send_cq;

	char* lrecv_buf;
 	struct ibv_mr* lmr_recv;
    	char* lsend_buf;
   	struct ibv_mr* lmr_send;
	
	char result_buf[RES_BUF_SIZE];
	char arg_buf[ARG_BUF_SIZE];

};

enum req_opcode {  RPC_FUNC_ECHO , RPC_FUNC_DONE, OPCODE_NUM};


#endif

