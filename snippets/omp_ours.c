void runGemm(T **a_a, T **b_a, T **c_a) {
    T *a = *a_a, *b = *b_a, *c = *c_a;
#pragma acc region for pcopy(c[i:i+N])   \
       pcopyin(a[i:i+N]) copyin(b[0:N*N])\
       hetero(1,adaptive)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; ++j) {
            c[(i * N) + j] *= B;
            for (int k = 0; k < N; ++k) {
                c[(i * N) + j] += A * a[(i*N) + k] * b[(k*N) + j];
            }
        }
    }
}
