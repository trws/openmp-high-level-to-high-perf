#pragma omp target device(smp,cuda)
void gemm(T *a, T *b, T *c, int i, T A, T B, int n);

#pragma omp target device(smp) copy_deps
#pragma omp task input ([n] a, [n*n] b) inout ([n] c)
void gemm(T *a, T *b, T *c, int i, T A, T B, int n) {
  for (int j = 0; j < n; ++j) {
    c[j] *= B;
    for (int k = 0; k < n; ++k) {
      c[j] += A * a[k] * b[(k*n)+j];
} } }

#pragma omp target device(cuda)
__global__ void 
cudag(T *a, T *b, T *c, int i, T A, T B, int n)
{ unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;
  if(j < n) {
    c[j] *= B;
    for (int k = 0; k < n; ++k) {
      c[j] += A * a[k] * b[(k*n)+j];
} } }

#pragma omp target device(cuda) copy_deps implements(gemm)
#pragma omp task input ([n] a, [n*n] b) inout ([n] c)
void gemm_gpu_wrap(T *a, T *b, T *c, int i, T A, T B, int n) 
{
  __global__ void 
  cudag(T *a, T *b, T *c, int i, T A, T B, int n);

  dim3 dB, dG;
  dB.x = 64; dB.y = dB.z = 1;
  dG.x = (n/dB.x)+1; dG.y = dG.z = 1;
  cudag<<<dG,dB>>>(a, b, c, i, A, B, n);
}

void runGemm(T **a, T **b, T **c) {
  for (int i = 0; i < N; ++i) {
    gemm(a[i], b[0], c[i], i, A, B, N);
  }
#pragma omp taskwait
}
