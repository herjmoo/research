#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

struct MPI_i {
  char kind;
  int rank_comm_with;
  int tag;
  int size;
  void *buffer;
  MPI_Request request_s;
  MPI_Request request_r;
};

/* MPI status data */
int i_req_push, i_req_pop;
struct MPI_i i_req[20];

/* variable for MPI information */
int rank_pmpi, size_pmpi;
unsigned long long identity_pmpi;
unsigned long long identity_gather_pmpi;
int byte0;

int getTotalSize (int count, MPI_Datatype datatype);

/* MPI_Init wrapper */
int MPI_Init (int *argc, char ***argv) {
  int res;

  /* connect to the original MPI_Init */
  res = PMPI_Init (argc, argv);

  /* get the MPI information (rank and size) */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank_pmpi);
  MPI_Comm_size(MPI_COMM_WORLD, &size_pmpi);

  /* initialize the statistics vaiables for each node */
  if (size_pmpi>64) {
    if (!rank_pmpi)
      fprintf(stderr, "ERROR: Cannot handle the total number of processes\n");
    PMPI_Finalize ();
    exit(1);
  }

  identity_pmpi = 1;
  identity_pmpi <<= rank_pmpi;

  i_req_push = i_req_pop = 0;
  byte0 = 0;

  return res;
}

void __attribute__ ((noinline)) PinMPI_Send (volatile int rank, volatile int dest, volatile int tag, volatile int bytes, volatile void *buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Recv (volatile int rank, volatile int source, volatile int tag, volatile int bytes, volatile void *buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Barrier (volatile int rank, volatile unsigned long long comm_with, volatile int bytes) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Bcast (volatile int rank, volatile unsigned long long comm_with, volatile int root, volatile int bytes, volatile void *buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Reduce (volatile int rank, volatile unsigned long long comm_with, volatile int root, volatile int bytes, volatile void *send_buf, volatile void *recv_buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Allreduce (volatile int rank, volatile unsigned long long comm_with, volatile int bytes, volatile void *send_buf, volatile void *recv_buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Allgather (volatile int rank, volatile unsigned long long comm_with, volatile int bytes, volatile void *send_buf, volatile void *recv_buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Alltoall (volatile int rank, volatile unsigned long long comm_with, volatile int bytes, volatile void *send_buf, volatile void *recv_buf) { bytes = 0; }
void __attribute__ ((noinline)) PinMPI_Alltoallv (volatile int rank, volatile unsigned long long comm_with, volatile int bytes, volatile void *send_buf, volatile void *recv_buf) { bytes = 0; }

/* MPI_Send wrapper */
int MPI_Send (void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
  int real_dest, bytes, res;
  MPI_Status status_r;

  PMPI_Send (&rank_pmpi, 1, MPI_INT, dest, 601, comm);
  PMPI_Recv (&real_dest, 1, MPI_INT, dest, 601, comm, &status_r);
  bytes = getTotalSize(count, datatype);

  /* connect to the original MPI_Send */
  res = PMPI_Send (buf, count, datatype, dest, tag, comm);

  /* point to pass arguments to pintool */
  PinMPI_Send (rank_pmpi, real_dest, tag, bytes, buf);

  return res;
}

/* MPI_Recv wrapper */
int MPI_Recv (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
  int real_source, bytes, res;
  MPI_Status status_r;

  PMPI_Recv (&real_source, 1, MPI_INT, source, 601, comm, &status_r);
  PMPI_Send (&rank_pmpi, 1, MPI_INT, source, 601, comm);
  bytes = getTotalSize(count, datatype);

  /* connect to the original MPI_Recv */
  res = PMPI_Recv (buf, count, datatype, source, tag, comm, status);

  /* point to pass arguments to pintool */
  PinMPI_Recv (rank_pmpi, real_source, tag, bytes, buf);

  return res;
}

/* MPI_Isend wrapper */
int MPI_Isend (void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) {
  i_req[i_req_push].kind = 's';
  i_req[i_req_push].tag = tag;
  i_req[i_req_push].size = getTotalSize(count, datatype);
  i_req[i_req_push].buffer = buf;
  PMPI_Isend (&rank_pmpi, 1, MPI_INT, dest, 601, comm, &(i_req[i_req_push].request_s));
  PMPI_Irecv (&(i_req[i_req_push].rank_comm_with), 1, MPI_INT, dest, 601, comm, &(i_req[i_req_push].request_r));
  i_req_push++;
  if (i_req_push==20) i_req_push = 0;

  /* connect to the original MPI_Isend */
  return PMPI_Isend (buf, count, datatype, dest, tag, comm, request);
}

/* MPI_Irecv wrapper */
int MPI_Irecv (void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
  i_req[i_req_push].kind = 'r';
  i_req[i_req_push].tag = tag;
  i_req[i_req_push].size = getTotalSize(count, datatype);
  i_req[i_req_push].buffer = buf;
  PMPI_Irecv (&(i_req[i_req_push].rank_comm_with), 1, MPI_INT, source, 601, comm, &(i_req[i_req_push].request_r));
  PMPI_Isend (&rank_pmpi, 1, MPI_INT, source, 601, comm, &(i_req[i_req_push].request_s));
  i_req_push++;
  if (i_req_push==20) i_req_push = 0;

  /* connect to the original MPI_Irecv */
  return PMPI_Irecv (buf, count, datatype, source, tag, comm, request);
}

/* MPI_Wait wrapper */
int MPI_Wait (MPI_Request *request, MPI_Status *status) {
  MPI_Status status_s, status_r;

  if (i_req[i_req_pop].kind=='s') {
    PMPI_Wait (&(i_req[i_req_pop].request_s), &status_s);
    PMPI_Wait (&(i_req[i_req_pop].request_r), &status_r);

    /* point to pass arguments to pintool */
    PinMPI_Send (rank_pmpi, i_req[i_req_pop].rank_comm_with, i_req[i_req_pop].tag, i_req[i_req_pop].size, i_req[i_req_pop].buffer);
  }
  else if (i_req[i_req_pop].kind=='r') {
    PMPI_Wait (&(i_req[i_req_pop].request_r), &status_r);
    PMPI_Wait (&(i_req[i_req_pop].request_s), &status_s);

    /* point to pass arguments to pintool */
    PinMPI_Recv (rank_pmpi, i_req[i_req_pop].rank_comm_with, i_req[i_req_pop].tag, i_req[i_req_pop].size, i_req[i_req_pop].buffer);
  }
  i_req_pop++;
  if (i_req_pop==20) i_req_pop = 0;

  /* connect to the original MPI_Wait */
  return PMPI_Wait (request, status);
}

/* MPI_Waitall wrapper */
int MPI_Waitall (int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]) {
  int i;
  MPI_Status status_s, status_r;

  for (i=0; i<count; i++) {
    if (i_req[i_req_pop].kind=='s') {
      PMPI_Wait (&(i_req[i_req_pop].request_s), &status_s);
      PMPI_Wait (&(i_req[i_req_pop].request_r), &status_r);

      /* point to pass arguments to pintool */
      PinMPI_Send (rank_pmpi, i_req[i_req_pop].rank_comm_with, i_req[i_req_pop].tag, i_req[i_req_pop].size, i_req[i_req_pop].buffer);
    }
    else if (i_req[i_req_pop].kind=='r') {
      PMPI_Wait (&(i_req[i_req_pop].request_r), &status_r);
      PMPI_Wait (&(i_req[i_req_pop].request_s), &status_s);

      /* point to pass arguments to pintool */
      PinMPI_Recv (rank_pmpi, i_req[i_req_pop].rank_comm_with, i_req[i_req_pop].tag, i_req[i_req_pop].size, i_req[i_req_pop].buffer);
    }
    i_req_pop++;
    if (i_req_pop==20) i_req_pop = 0;
  }

  /* connect to the original MPI_Waitall */
  return PMPI_Waitall (count, array_of_requests, array_of_statuses);

}

/* MPI_Barrier wrapper */
int MPI_Barrier (MPI_Comm comm) {
  int res;

  /* connect to the original MPI_Barrier */
  res = PMPI_Barrier (comm);
  
  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);

  /* point to pass arguments to pintool */
  PinMPI_Barrier (rank_pmpi, identity_gather_pmpi, byte0);

  return res;
}


/* MPI_Bcast wrapper */
int MPI_Bcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
  int bytes, res;

  /* connect to the original MPI_Bcast */
  res = PMPI_Bcast (buffer, count, datatype, root, comm);

  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  bytes = getTotalSize(count, datatype);

  /* point to pass arguments to pintool */
  PinMPI_Bcast (rank_pmpi, identity_gather_pmpi, root, bytes, buffer);

  return res;
}

/* MPI_Reduce wrapper */
int MPI_Reduce (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
  int bytes, res;

  /* connect to the original MPI_Reduce */
  res = PMPI_Reduce (sendbuf, recvbuf, count, datatype, op, root, comm);
  
  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  bytes = getTotalSize(count, datatype);

  /* point to pass arguments to pintool */
  PinMPI_Reduce (rank_pmpi, identity_gather_pmpi, root, bytes, sendbuf, recvbuf);

  return res;
}

/* MPI_Allreduce wrapper */
int MPI_Allreduce (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  int bytes, res;

  /* connect to the original MPI_Allreduce */
  res = PMPI_Allreduce (sendbuf, recvbuf, count, datatype, op, comm);
  
  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  bytes = getTotalSize(count, datatype);

  /* point to pass arguments to pintool */
  PinMPI_Allreduce (rank_pmpi, identity_gather_pmpi, bytes, sendbuf, recvbuf);

  return res;
}

/* MPI_Allgather wrapper */
int MPI_Allgather (void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
  int bytes, res;

  /* connect to the original MPI_Allgather */
  res = PMPI_Allgather (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    
  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  bytes = getTotalSize(sendcount, sendtype);

  /* point to pass arguments to pintool */
  PinMPI_Allgather (rank_pmpi, identity_gather_pmpi, bytes, sendbuf, recvbuf);

  return res;
}

/* MPI_Alltoall wrapper */
int MPI_Alltoall (void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
  int bytes, res;

  /* connect to the original MPI_Alltoall */
  res = PMPI_Alltoall (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);

  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  bytes = getTotalSize(sendcount, sendtype);

  /* point to pass arguments to pintool */
  PinMPI_Alltoall (rank_pmpi, identity_gather_pmpi, bytes, sendbuf, recvbuf);

  return res;
}

// MPI_Alltoallv wrapper
int MPI_Alltoallv (void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) {
  int i, local_size, bytes, res;

  // connect to the original MPI_Alltoallv
  res = PMPI_Alltoallv (sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);
  
  PMPI_Allreduce (&identity_pmpi, &identity_gather_pmpi, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, comm);
  
  MPI_Comm_size(comm, &local_size);
  bytes = 0;
  for (i=0; i<local_size; i++)
    bytes += getTotalSize(sendcnts[i], sendtype);

  /* point to pass arguments to pintool */
  PinMPI_Alltoallv (rank_pmpi, identity_gather_pmpi, bytes, sendbuf, recvbuf);

  return res;
}

int getTotalSize (int count, MPI_Datatype datatype) {
  int size = 0;

  if (datatype==MPI_CHAR || datatype==MPI_UNSIGNED_CHAR || datatype==MPI_BYTE)
    size = 1;
  else if (datatype==MPI_SHORT || datatype==MPI_UNSIGNED_SHORT)
    size = 2;
  else if (datatype==MPI_INT || datatype==MPI_UNSIGNED || datatype==MPI_FLOAT)
    size = 4;
  else if (datatype==MPI_LONG || datatype==MPI_UNSIGNED_LONG || datatype==MPI_LONG_LONG || datatype==MPI_DOUBLE)
    size = 8;
  else if (datatype==MPI_LONG_DOUBLE)
    size = 16;
  else if (datatype==MPI_CHARACTER || datatype==MPI_INTEGER1)
    size = 1;
  else if (datatype==MPI_INTEGER2)
    size = 2;
  else if (datatype==MPI_INTEGER || datatype==MPI_INTEGER4 || datatype==MPI_REAL || datatype==MPI_REAL4 || datatype==MPI_LOGICAL)
    size = 4;
  else if (datatype==MPI_INTEGER8 || datatype==MPI_REAL8 || datatype==MPI_DOUBLE_PRECISION || datatype==MPI_COMPLEX)
    size = 8;
  else if (datatype==MPI_DOUBLE_COMPLEX)
    size = 16;
  else
    fprintf(stderr, "[node %d] ERROR: No handle datatype (%d)\n", rank_pmpi, (int) datatype);

  return size*count;
}

