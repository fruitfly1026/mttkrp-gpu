#ifndef _CONVERT_H
#define _CONVERT_H
#include "readtensor.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>

template <typename T>
struct soa_tensor {
 public:
  unsigned int *i;
  unsigned int *j;
  unsigned int *k;
  T *val;
  int nnz;
  soa_tensor(int length): nnz(length) {
    i = (unsigned int *)malloc(sizeof(unsigned int) * length);
    j = (unsigned int *)malloc(sizeof(unsigned int) * length);
    k = (unsigned int *)malloc(sizeof(unsigned int) * length);
    val = (T *)malloc(sizeof(T) * length);
  }
};

#ifdef DOUBLE
typedef soa_tensor<double> stensor;
typedef double mtype;
// typedef soa_tensor<double> *tensor;
#else
typedef soa_tensor<float> stensor;
typedef float mtype;
#endif

// typedef unsigned long type_thread;
#ifdef CHAR
  typedef uint8_t type_thread;
#endif
#ifdef SHORT
  typedef uint16_t type_thread;
#endif
#ifdef INT
  typedef uint32_t type_thread;
#endif
#ifdef LONG
  typedef uint64_t type_thread;
#endif

void convert(tensor data, stensor &CPU_tensor, int nnz, int mode);
#endif