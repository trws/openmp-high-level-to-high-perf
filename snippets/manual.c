void runGemm(T **a_a, T **b_a, T **c_a) {
  ctsar * s = NULL; int div = 10;
  ctsar_init(&s,N,CTSAR_ADAPTIVE,CTSAR_DEV_ALL,NULL,&div);
#pragma omp parallel default(shared)
  do{
    T *a = ctsar_reg_mem(s, a_a[0], sizeof(T)*N, N,
            CTSAR_MEM_PARTIAL | CTSAR_MEM_INPUT);
    T *b = ctsar_reg_mem(s, b_a[0], sizeof(T)*N, N,
            CTSAR_MEM_INPUT);
    T *c = ctsar_reg_mem(s, c_a[0], sizeof(T)*N, N,
            CTSAR_MEM_PARTIAL | CTSAR_MEM_INOUT);
    ctsar_next(s,N);
#pragma acc region for deviceptr(a,b,c) independent \
        if(ctsar_get_type(s) == CTSAR_DEV_GPU)
    for (int i = CSTART(s); i < CEND(s); ++i) {
      for (int j = 0; j < N; ++j) {
        c[(i * NJ) + j] *= B;
        for (int k = 0; k < N; ++k)
          c[(i*N) + j] += A * a[(i*N) + k] * b[(k*N) + j];
    } }
  }while(ctsar_loop(s));
}
