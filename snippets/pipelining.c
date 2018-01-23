#pragma omp target teams distribute \
  pipeline(static) \
  map(pipeline,to: A[k-1:3][0:ny-1][0:nx-1]) \
  map(pipeline,from: Anext[k:1][0:ny-1][0:nx-1])
for(k=1;k<nz-1;k++) {
  #pragma omp parallel for
  for(i=1;i<nx-1;i++) {
    for(j=1;j<ny-1;j++) {
      Anext[Index3D (i,     j,     k)] =
         (A[Index3D (i,     j,     k + 1)] +
          A[Index3D (i,     j,     k - 1)] +
          A[Index3D (i,     j + 1, k)] +
          A[Index3D (i,     j - 1, k)] +
          A[Index3D (i + 1, j,     k)] +
          A[Index3D (i - 1, j,     k)])*c1
        - A[Index3D (i,     j,     k)]*c0;
    } } 
}
