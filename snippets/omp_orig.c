void runGemm(T **a, T **b, T **c) {
#pragma omp parallel for
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      c[i][j] *= B;
      for (int k = 0; k < N; ++k) {
        c[i][j] += A * a[i][k] * b[k][j];
      }
    }
  }
}
