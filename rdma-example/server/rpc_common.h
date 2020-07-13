#ifndef __RPC_COMMON_H__
#define __RPC_COMMON_H__

struct ib_info_t {
    int lid;
    int qpn;
    int mkey_args;
    long long addr_args;
    int mkey_result;
    long long addr_result;
};


enum req_opcode {  RPC_FUNC_ECHO , RPC_FUNC_DONE, OPCODE_NUM};

#define IB_PORT_SERVER 1
#define IB_PORT_CLIENT 1
#define MAX_RECV_WQES 128

#define ARG_BUF_SIZE 10
#define RES_BUF_SIZE 10

#define TCP_PORT_OFFSET 23456
#define TCP_PORT_RANGE 1000


#endif

