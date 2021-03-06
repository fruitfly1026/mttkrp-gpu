#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <stdio.h>
#include "splatt.h"
#include "readtensor.h"
#include "convert.h"
#include "matrix.h"
#include "test.h"
#include "MTTKRP.h"
#include "gpuMTTKRP.h"

using std::cout;
using std::cerr;
using std::endl;

using std::string;
// using std::vector;

using std::sort;




class sorter {
 public:
  sorter (const int &_sort_order): sort_order(_sort_order) {}

  bool operator()(const item &one, const item &two) const {

    int idx0 = (sort_order) % 3;
    int idx1 = (idx0 + 1) % 3;
    int idx2 = (idx0 + 2) % 3;

    if (one.coord[idx0] != two.coord[idx0]) {
      return one.coord[idx0]  < two.coord[idx0];
    } else if (one.coord[idx1] != two.coord[idx1]) {
      return one.coord[idx1] < two.coord[idx1];
    } else {
      return one.coord[idx2] < two.coord[idx2];
    }
    return false;
  }
 private:
  int sort_order;
};


int main(int argc, char **argv) {
  int dim_i = 0;
  int dim_j = 0;
  int dim_k = 0;

  int R = 16;
  int BLOCK_SIZE = 1024;
  int mode=0;


  if(argc == 1) {
    printf("Usage: ./main [tensor] R BLOCK_SIZE mode\n");
    return -1;
  }
  char *in_file = argv[1];
  if (argc > 2 and atoi(argv[2])) {
    R = atoi(argv[2]);
  }
  if (argc > 3 and atoi(argv[3])) {
    BLOCK_SIZE = atoi(argv[3]);
  }

  if(argc > 4 and atoi(argv[4])){
    mode=atoi(argv[4]);
    if(mode>=3){
      printf("Error, the mode parameter is only allowed into 0-2\n");
    }
  }

  printf("Input parameters:\n");
  printf("\tR=%d\n", R);
  printf("\tBLOCK_SIZE=%d\n", BLOCK_SIZE);
  printf("\tmode=%d\n", mode);
  printf("\n");

  struct timeval start;
  struct timeval end;
  int nRows1, nRows2;

  int nnz = precess(dim_i, dim_j, dim_k, in_file);
  printf("I=%d J=%d k=%d\n", dim_i, dim_j, dim_k);  // TTM multiply on third dimension, MTTKRP multiply on second and third dimension

  tensor data;
  tensor_malloc(&data, nnz);

  readtensor(data, in_file);

  sorter compare(0);
  sort(data,data+nnz,compare);
  // test(data, nnz);

  stensor H_Tensor(nnz);
  convert(data, H_Tensor, nnz, mode);



  ttype *A,*B, *C; // B,C for MTTKRP

  if(mode==0){
    nRows1=dim_j+1;
    nRows2=dim_k+1;
  }
  else if(mode==1){
    nRows1=dim_k+1;
    nRows2=dim_i+1;
  }
  else {
    nRows1=dim_i+1;
    nRows2=dim_j+1;
  }

  genMatrix(&B, nRows1, R);
  randomFill(B, nRows1, R);
  genMatrix(&C, nRows2, R);
  randomFill(C, nRows2, R);



  semitensor rtensor;
  int *flag;
  int nfibs = preprocess(H_Tensor, &flag, rtensor);
  printf("nfibs=%d\n", nfibs);

  gettimeofday(&start, NULL);
  /* CPU MTTKRP with no flags */
  MTTKRP(H_Tensor, nfibs, B, C, R, rtensor);
  gettimeofday(&end, NULL);

  float CPU_time2 = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
  printf("CPU_time MTTKRP: %f s\n", CPU_time2);


  // test_TTM(rtensor);
  unsigned char type = 0; // type of bit_flag
  /* GPU MTTKRP using FCOO format */
  ttype *d_result = callTTM(H_Tensor, B, C, nRows1, nRows2, R, rtensor, type, BLOCK_SIZE);


  verify(rtensor, d_result);

  tensor_free(data);


}

