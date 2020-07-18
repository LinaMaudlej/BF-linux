CC=gcc
CFLAGS_GCC=-Wall -std=gnu11
CFLAGS=-g -lineinfo -O3
CUDAFLAGS=-maxrregcount=32
LDFLAGS=-libverbs

all: client_rdma

client_rdma : client.o          
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.cu
	nvcc $(CFLAGS) $(CUDAFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_GCC) -c -o $@ $<

clean:
	rm -f *.o client_rdma 

