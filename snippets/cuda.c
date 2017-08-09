__global__ void 
cudag(T *a, T *b, T *c, T A, T B, int n) {
  uint i = blockIdx.x * blockDim.x + threadIdx.x;
  if(i < n) {
    for (int j = 0; j < n; ++j) {
      c[(i*N) + j] *= B;
      for (int k = 0; k < n; ++k) {
        c[(i*N)+j] += A * a[(i*N)+k] * b[(k*N)+j];
} } } }
void runGemm(T **a, T **b, T **c) {
  T *ca, *cb, *cc; dim3 dB, dG;
  size_t size = N*N*sizeof(T);
  dB.x = 64; dB.y = dB.z = 1;
  dG.x = (N/dB.x)+1; dG.y = dG.z = 1;
  cudaMalloc(&ca, size);
  cudaMalloc(&cb, size);
  cudaMalloc(&cc, size);
  cudaMemcpy(ca,*a,size,cudaMemcpyHostToDevice);
  cudaMemcpy(cb,*b,size,cudaMemcpyHostToDevice);
  cudaMemcpy(cc,*c,size,cudaMemcpyHostToDevice);
  cudag<<<dG,dB>>>(a, b, c, A, B, N);
  cudaMemcpy(*c,cc,size,cudaMemcpyDeviceToHost);
}
