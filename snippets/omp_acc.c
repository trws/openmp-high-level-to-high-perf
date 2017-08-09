void runGemm(T **a_a, T **b_a, T **c_a) {
  T *a = *a_a, *b = *b_a, *c = *c_a;
//OpenMP
#pragma omp parallel for 
//Accelerated OpenMP
#pragma acc region for copy(c[0:N*N])   \
        copyin(a[0:N*N],b[0:N*N])
//Accelerated OpenMP + extension
#pragma acc region for part_copy(c[1:N][0:N])\
        copyin(b[0:N*N]) part_copyin(a[1:N][0:N])\
        hetero(1, all, adaptive)
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      c[(i*N) + j] *= B;
      for (int k = 0; k < N; ++k) {
          c[(i*N)+j] += A * a[(i*N)+k] * b[(k*N)+j];
} } } }
