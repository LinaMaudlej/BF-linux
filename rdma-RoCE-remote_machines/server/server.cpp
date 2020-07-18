#include <infiniband/verbs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "rpc_common.h"


int get_gid_index(ibv_context* dev)
{
	return 3;
	/*for (int i = 0; i < 0xffff; ++i) {
	  ibv_gid gid;

	  if (ibv_query_gid(dev, 1, i, &gid)) {
	  printf("ibv_query_gid failed for gid %d", i);
	  exit(1);
	  }

	  if (gid.global.subnet_prefix != 0ull ||
	  (gid.global.interface_id & 0xffffffff) != 0xffff0000ull)
	  continue;

	  char gid_type_str[7];
	  int len = ibv_read_sysfs_file("/sys/class/infiniband/mlx5_0/ports/1/gid_attrs/types",
	  boost::lexical_cast<string>(i).c_str(), gid_type_str, sizeof(gid_type_str));
	  if (len < 0) {
	  printf("cannot read gid type for gid %d", i);
	  return -1;
	  }

	  if (strncasecmp(gid_type_str, "RoCE v2", len) != 0)
	  continue;

	  return i;
	  }
	  return -1;*/
}
/*
   struct ibv_context *ibv_open_device_by_name(const std::string& device_name)
   {
   int num_devices = 0;
   struct ibv_device **devices_list = ibv_get_device_list(&num_devices);
   if(!devices_list){
   printf("ERROR: ibv_get_device_list() failed\n");
   exit(1);
   }
   std::cout << "ibv_open_device_by_name: " << num_devices << " devices were found." << std::endl;
   std::cout << "device_name is " << device_name << std::endl;
   for (int i = 0; i < num_devices; ++i) {
   string cur_name = ibv_get_device_name(devices_list[i]);
   std::cout << "device[" << i << "] name: " << cur_name << std::endl;
   if (device_name == cur_name){
   std::cout << device_name << " found." << std::endl;
   return ibv_open_device(devices_list[i]);
   }
   }
   }*/


int do_echo(char *arg_buf, char *result_buf) {
	printf("echo...\n");
	/*int i=0;
	  for (i = 0; i< RES_BUF_SIZE ; i++) {
	  result_buf[i]=arg_buf[i];
	  }
	  printf ("the value is %c\n",arg_buf[0]);*/
	return RES_BUF_SIZE;
}
struct ib_resources_t *setup_connection(int tcp_port) {

	struct ib_resources_t *ib_resources = (struct ib_resources_t*) malloc(sizeof(struct ib_resources_t));



	/* setup a TCP connection for initial negotiation with client */
	int lfd, sfd;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd < 0) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(tcp_port);

	if (bind(lfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(1);
	}

	listen(lfd, 1);

	printf("Server waiting on port %d. Client can connect\n", tcp_port);
	sfd = accept(lfd, NULL, NULL);
	if (sfd < 0) {
		perror("accept");
		exit(1);
	}
	printf("client connected\n");

	/* now that client has connected to us via TCP we'll open up some Infiniband resources and send it the parameters */

	/* get device list */
	struct ibv_device **device_list = ibv_get_device_list(NULL);
	if (!device_list) {
		printf("ERROR: ibv_get_device_list failed\n");
		exit(1);
	}

	/* select first (and only) device to work with */
	struct ibv_context *context = ibv_open_device(device_list[1]);

	/* create protection domain (PD) */
	struct ibv_pd *pd = ibv_alloc_pd(context);
	if (!pd) {
		printf("ERROR: ibv_alloc_pd() failed\n");
		exit(1);
	}
	struct ibv_mr *mr_recv;
	char *recv_buf=(char*)malloc(HOST_TOTAL_DATA_FROM_CLIENT_SIZE);
	mr_recv = ibv_reg_mr(pd, recv_buf, HOST_TOTAL_DATA_FROM_CLIENT_SIZE, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
	if (!mr_recv) {
		printf("ibv_reg_mr() failed for data_for_host\n");
		exit(1);
	}

	struct ibv_mr *mr_send;
       char *send_buf=(char*)malloc(HOST_TOTAL_DATA_TO_CLIENT_SIZE);
	mr_send = ibv_reg_mr(pd, send_buf, HOST_TOTAL_DATA_FROM_CLIENT_SIZE, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
	if (!mr_send) {
		printf("ibv_reg_mr() failed for data_from_host\n");
		exit(1);
	}

	struct ibv_cq *recv_cq = ibv_create_cq(context, HOST_RECV_CQ_SIZE, NULL, NULL, 0);
	if (!recv_cq) {
		printf("ERROR: ibv_create_cq() failed\n");
		exit(1);
	}



	struct ibv_cq *send_cq = ibv_create_cq(context, HOST_SEND_CQ_SIZE, NULL, NULL, 0);
	if (!send_cq) {
		printf("ibv_create_cq() failed\n");
		exit(1);
	}    
	/* allocate a memory region for the RPC arguments.
	 *      * must be writeable by client */
	struct ibv_mr *mr_args;
	//	char *arg_buf = (char*)malloc(ARG_BUF_SIZE);

	mr_args = ibv_reg_mr(pd,ib_resources->arg_buf, ARG_BUF_SIZE, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
	if (!mr_args) {
		printf("0) ibv_reg_mr() failed for argument buffer args  \n");
		exit(1);
	}
	/* allocate a memory region for the RPC result.
	 *      * must be readable by client */
	struct ibv_mr *mr_result;
	//char *result_buf = (char*)malloc(RES_BUF_SIZE);
	mr_result = ibv_reg_mr(pd,ib_resources->result_buf, RES_BUF_SIZE, IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE);
	if (!mr_result) {
		printf(" 1) ibv_reg_mr() failed for result buffer resuls \n");
		exit(1);
	}

	/* create completion queue (CQ). We'll use same CQ for both send and receive parts of the QP */
	struct ibv_cq *cq = ibv_create_cq(context, 100, NULL, NULL, 0); /* create a CQ with place for 100 CQEs */
	if (!cq) {
		printf("2) ERROR: ibv_create_cq() failed\n");
		exit(1);
	}

	/* create QP */
	struct ibv_qp_init_attr qp_init_attr;
	memset(&qp_init_attr, 0, sizeof(struct ibv_qp_init_attr));
	qp_init_attr.send_cq = cq;
	qp_init_attr.recv_cq = cq;
	qp_init_attr.qp_type = IBV_QPT_RC; /* we'll use RC transport service, which supports RDMA */
	qp_init_attr.cap.max_send_wr = 2; //changed/* max of 1 WQE in-flight in SQ. that's enough for us */
	qp_init_attr.cap.max_recv_wr = MAX_RECV_WQES; /* max of 8 WQE's in-flight in RQ. that's more than enough for us */
	qp_init_attr.cap.max_send_sge = 2; //changed /* 1 SGE in each send WQE */
	qp_init_attr.cap.max_recv_sge = 2; //changed /* 1 SGE in each recv WQE */
	struct ibv_qp *qp = ibv_create_qp(pd, &qp_init_attr);
	if (!qp) {
		printf("ERROR: ibv_create_qp() failed\n");
		exit(1);
	}

	/* ok, before we continue we need to get info about the client' QP, and send it info about ours.
	 *      * namely: QP number, and LID.
	 *           * we'll use the TCP socket for that */

	/* first query port for its LID (L2 address) */
	int ret;
	struct ibv_port_attr port_attr;
	ret = ibv_query_port(context, IB_PORT_SERVER, &port_attr);
	if (ret) {
		printf("ERROR: ibv_query_port() failed\n");
		exit(1);
	}

	/* now send our info to client */
	struct ib_info_t my_info;
	my_info.lid = port_attr.lid;
	my_info.qpn = qp->qp_num;
	my_info.mkey_args = mr_args->rkey;
	my_info.addr_args = (uintptr_t)mr_args->addr;
	my_info.mkey_result = mr_result->rkey;
	my_info.addr_result = (uintptr_t)mr_result->addr;
	int gid_index = get_gid_index(context);
	if (ibv_query_gid(context, 1, gid_index, &(my_info.gid) )) {
		printf("ibv_query_gid failed for gid \n");
		exit(1);
	}

	ret = send(sfd, &my_info, sizeof(struct ib_info_t), 0);
	if (ret < 0) {
		perror("send");
		exit(1);
	}
	/* get client's info */
	struct ib_info_t client_info;
	recv(sfd, &client_info, sizeof(struct ib_info_t), 0);
	if (ret < 0) {
		perror("recv");
		exit(1);
	}


	/* we don't need TCP anymore. kill the socket */


	close(sfd);
	close(lfd);
	/* now need to connect the QP to the client's QP.
	 *      * this is a multi-phase process, moving the state machine of the QP step by step
	 *           * until we are ready */
	struct ibv_qp_attr qp_attr;

	/*QP state: RESET -> INIT */
	memset(&qp_attr, 0, sizeof(struct ibv_qp_attr));
	qp_attr.qp_state = IBV_QPS_INIT;
	qp_attr.pkey_index = 0;
	qp_attr.port_num = IB_PORT_SERVER;
	qp_attr.qp_access_flags = IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ; /* we'll allow client to RDMA write and read on this QP */
	ret = ibv_modify_qp(qp, &qp_attr, IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS);
	if (ret) {
		printf("ERROR: ibv_modify_qp() to INIT failed\n");
		exit(1);
	}


	/*QP: state: INIT -> RTR (Ready to Receive) */
	memset(&qp_attr, 0, sizeof(struct ibv_qp_attr));
	qp_attr.qp_state = IBV_QPS_RTR;
	qp_attr.path_mtu = IBV_MTU_4096;
	qp_attr.dest_qp_num = client_info.qpn; /* qp number of client */
	qp_attr.rq_psn      = 0 ;
	qp_attr.max_dest_rd_atomic = 1; //changed/* max in-flight RDMA reads */
	qp_attr.min_rnr_timer = 12;
	qp_attr.ah_attr.is_global = 1; /* No Network Layer (L3) */
	qp_attr.ah_attr.grh.dgid = client_info.gid;
	qp_attr.ah_attr.grh.sgid_index =get_gid_index(context);// get_gid_index(context);
	qp_attr.ah_attr.grh.flow_label = 0;
	qp_attr.ah_attr.grh.hop_limit = 1;
	qp_attr.ah_attr.grh.traffic_class = 0;

	qp_attr.ah_attr.dlid = client_info.lid; /* LID (L2 Address) of client */
	qp_attr.ah_attr.sl = 0;
	qp_attr.ah_attr.src_path_bits = 0;
	qp_attr.ah_attr.port_num = IB_PORT_SERVER;
	ret = ibv_modify_qp(qp, &qp_attr, IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER);
	if (ret) {
		printf("ERROR: ibv_modify_qp() to RTR failed with ret %d\n",ret);
		exit(1);
	}
	/*QP: state: RTR -> RTS (Ready to Send) */
	memset(&qp_attr, 0, sizeof(struct ibv_qp_attr));
	qp_attr.qp_state = IBV_QPS_RTS;
	qp_attr.sq_psn = 0;
	qp_attr.timeout = 14;
	qp_attr.retry_cnt = 7; //100
	qp_attr.rnr_retry = 7;
	qp_attr.max_rd_atomic = 1; //changed
	ret = ibv_modify_qp(qp, &qp_attr, IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC);
	if (ret) {
		printf("ERROR: ibv_modify_qp() to RTS failed\n");
		exit(1);
	}

	/* now let's populate the receive QP with recv WQEs */
	struct ibv_recv_wr recv_wr; /* this is the receive work request (the verb's representation for receive WQE) */
	int i;
	for (i = 0; i < MAX_RECV_WQES; i++) {
		memset(&recv_wr, 0, sizeof(struct ibv_recv_wr));
		recv_wr.wr_id = i;
		recv_wr.sg_list = NULL;
		recv_wr.num_sge = 0;
		if (ibv_post_recv(qp, &recv_wr, NULL)) {
			printf("ERROR: ibv_post_recv() failed\n");
			exit(1);
		}
	}

	ib_resources->context = context;
	ib_resources->pd = pd;
	ib_resources->qp = qp;
	ib_resources->send_cq = send_cq;
	ib_resources->recv_cq = recv_cq;

	ib_resources->lrecv_buf = recv_buf;
	ib_resources->lmr_recv = mr_recv;
	ib_resources->lsend_buf = send_buf;
	ib_resources->lmr_send = mr_send;

	return ib_resources; 

}


void teardown_connection(struct ib_resources_t *ib_resources) {
	ibv_destroy_qp(ib_resources->qp);
	ibv_destroy_cq(ib_resources->send_cq);
	ibv_destroy_cq(ib_resources->recv_cq);


	ibv_dealloc_pd(ib_resources->pd);
	ibv_close_device(ib_resources->context);
}


int main(int argc, char *argv[]) {

	int tcp_port = TCP_PORT_OFFSET + (rand() % TCP_PORT_RANGE); /* to avoid conflicts with other users of the machine */


	struct ib_resources_t *ib_resources = setup_connection(tcp_port);

	/* now finally we get to the actual work */
	/* so the protocol goes like this:
	 * 1. we'll wait for a CQE indicating that we got an RDMA Write with Immediate from the client.
	 * this tells us we have new work to do.
	 * The immediate value (written in the CQE) will tell us what function we will be performing
	 * 2. now we know that the arguments are in arg_buf.
	 * so we perform the function (from 1) on the arguments (from 2).
	 * 3. we store the result in result_buf
	 * 4. we send an RDMA send with immediate to the client (with no data),
	 * telling it that the result is ready for reading.
	 * 5. we go back to 1 to perform another operation
	 * */


	for (int i = 0; ; i++) {
		/*step 1: poll for CQE */
		struct ibv_wc wc;
		int ncqes;
		do {
			ncqes = ibv_poll_cq(ib_resources->recv_cq, 1, &wc);
		} while (ncqes == 0);
		if (ncqes < 0) {
			printf("ERROR: ibv_poll_cq() failed\n");
			exit(1);
		}
		if (wc.status != IBV_WC_SUCCESS) {
			printf("ERROR: got CQE with error %d (line %d)\n", wc.status, __LINE__);
			exit(1);
		}

		assert(wc.opcode == IBV_WC_RECV_RDMA_WITH_IMM);

		printf("Got a CQE :)  wc.status=%d  wc.opcode=%d wc.byte_len=%d wc.wr_id=%d \n",wc.status, wc.opcode, wc.byte_len, wc.wr_id);

		/*for(int i = 0 ; i < wc.byte_len ; i++) {
		 *                 printf("%d ",*((char*)virt_addr1+i));
		 *                         }*/

		/* perform RPC and store result in result_buf */
		int res_size = 0;

		if(wc.imm_data==RPC_FUNC_DONE){
			printf("client says it's done. we'll exit..\n");

			break;
		}
		switch (wc.imm_data){


			case RPC_FUNC_ECHO:           { 
							      printf("will perform function: echo\n");
							      res_size = do_echo(ib_resources->arg_buf, ib_resources->result_buf);
							      break;
						      }

			default:
						      printf("unexpected immediate value %d\n", wc.imm_data);
						      exit(1);
		}



		/* send RDMA send with immediate to client to tell it we're done */
		struct ibv_send_wr send_wr;
		struct ibv_send_wr *bad_send_wr;
		memset(&send_wr, 0, sizeof(struct ibv_send_wr));
		send_wr.wr_id = i;
		send_wr.sg_list = NULL;
		send_wr.num_sge = 0;
		send_wr.opcode = IBV_WR_SEND_WITH_IMM;
		send_wr.send_flags = IBV_SEND_SIGNALED;
		send_wr.imm_data = res_size;

		if (ibv_post_send(ib_resources->qp, &send_wr, &bad_send_wr)) {
			printf("ERROR: ibv_post_send() failed\n");
			exit(1);
		}

		/* now poll CQ for completion of our RDMA send with immediate.*/
		do {
			ncqes = ibv_poll_cq(ib_resources->recv_cq, 1, &wc);
		} while (ncqes == 0);
		if (ncqes < 0) {
			printf("ERROR: ibv_poll_cq() failed\n");
			exit(1);
		}
		if (wc.status != IBV_WC_SUCCESS) {
			printf("ERROR: got CQE with error %d (line %d)\n", wc.status, __LINE__);
			exit(1);
		}
		assert(wc.opcode == IBV_WC_SEND);

		/* we notified client. now it should read the result and then we go again */
	}


	printf("Done\n");
	/* cleanup */

	teardown_connection(ib_resources);
	return 0;
}
