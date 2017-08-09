/**
 * gemm.c: This file is part of the PolyBench/GPU 1.0 test suite.
 *
 *
 * Contact: Scott Grauer-Gray <sgrauerg@gmail.com>
 * Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://www.cse.ohio-state.edu/~pouchet/software/polybench/GPU
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../../../common/polybenchUtilFuncts.h"
#include "../../../../../common/ctsar.h"
ctsar * s = NULL;

//define the error threshold for the results "not matching"
#define PERCENT_DIFF_ERROR_THRESHOLD 0.05

/* Problem size. */
#define NI 2048
#define NJ 2048
#define NK 2048

/* Can switch DATA_TYPE between float and double */
typedef float DATA_TYPE;


void runGemm_gpu(DATA_TYPE **a_a, DATA_TYPE **b_a, DATA_TYPE **c_a)
{
    int i, j, k;
    DATA_TYPE p_alpha = 32412;
    DATA_TYPE p_beta = 2123;

    /* C := alpha*A*B + beta*C */
#pragma omp parallel default(shared) private(j,i,k)
    {
        int tid = omp_get_thread_num();

        do{
            ctsar_next(s,NI);
            int gts = CSTART(s,tid);
            int gte = CEND(s,tid);
            DATA_TYPE *a,*b,*c;
            a = ctsar_reg_mem(s, a_a[0], sizeof(DATA_TYPE)*NJ, NI,
                    CTSAR_MEM_PARTIAL | CTSAR_MEM_INPUT);
            b = ctsar_reg_mem(s, b_a[0], sizeof(DATA_TYPE)*NJ, NI,
                    CTSAR_MEM_INPUT);
            c = ctsar_reg_mem(s, c_a[0], sizeof(DATA_TYPE)*NJ, NI,
                    CTSAR_MEM_PARTIAL | CTSAR_MEM_INPUT | CTSAR_MEM_OUTPUT);

            ctsar_start(s);
#pragma acc data region deviceptr(a,b,c) if(ctsar_get_type(s) == CTSAR_DEV_GPU)
            {
#pragma acc region for private(i,j,k) independent if(ctsar_get_type(s) == CTSAR_DEV_GPU)
                for (i = gts; i < gte; ++i)
                {
#pragma acc for independent
                    for (j = 0; j < NJ; ++j)
                    {
                        c[(i * NJ) + j] *= p_beta;

                        for (k = 0; k < NK; ++k)
                        {
                            c[(i * NJ) + j] += p_alpha * a[(i*NJ) + k] * b[(k*NJ) + j];
                        }
                    }
                }
            }
            ctsar_end(s);
            /* printf("here: %d\n", omp_get_thread_num()); */
        }while(ctsar_loop(s));
    }
}

void runGemm(DATA_TYPE **a, DATA_TYPE **b, DATA_TYPE **c)
{
    int i, j, k;
    DATA_TYPE p_alpha = 32412;
    DATA_TYPE p_beta = 2123;

    /* C := alpha*A*B + beta*C */
#pragma omp parallel for private(i,j,k) schedule(runtime)
    for (i = 0; i < NI; i++)
    {
        for (j = 0; j < NJ; j++)
        {
            c[i][j] *= p_beta;

            for (k = 0; k < NK; ++k)
            {
                c[i][j] += p_alpha * a[i][k] * b[k][j];
            }
        }
    }
}



void init(DATA_TYPE **A, DATA_TYPE **B, DATA_TYPE **C, DATA_TYPE **C_outputFromGpu)
{
    int i, j;

    for (i = 0; i < NI; i++)
    {
        for (j = 0; j < NK; j++)
        {
            A[i][j] = ((DATA_TYPE) i*j) / NI;
        }
    }

    for (i = 0; i < NK; i++)
    {
        for (j = 0; j < NJ; j++)
        {
            B[i][j] = ((DATA_TYPE) i*j + 1) / NJ;
        }
    }

    for (i = 0; i < NI; i++)
    {
        for (j = 0; j < NJ; j++)
        {
            C[i][j] = ((DATA_TYPE) i*j + 2) / NJ;
            C_outputFromGpu[i][j] = ((DATA_TYPE) i*j + 2) / NJ;
        }
    }
}


void compareResults(DATA_TYPE **C, DATA_TYPE **C_outputFromGpu)
{
    int i, j, fail;
    fail = 0;

    // Compare C1 and C2
    for (i=0; i < NI; i++) 
    {
        for (j=0; j < NJ; j++) 
        {
            if (percentDiff(C[i][j], C_outputFromGpu[i][j]) > PERCENT_DIFF_ERROR_THRESHOLD) 
            {
                if(fail < 10){
                    printf("CPU: %f GPU: %f\n", C[i][j], C_outputFromGpu[i][j]);
                }
                fail++;
            }
        }
    }

    // Print results
    printf("Non-Matching CPU-GPU Outputs Beyond Error Threshold of %4.2f Percent: %d\n", PERCENT_DIFF_ERROR_THRESHOLD, fail);

}



int main(int argc, char** argv)
{
    double t_start, t_end;

    /* Array declaration */
    DATA_TYPE alpha;
    DATA_TYPE beta;
    DATA_TYPE **C = (DATA_TYPE**)ctsar_alloc_2d(sizeof(DATA_TYPE),NI,NJ);
    DATA_TYPE **C_outputFromGpu = (DATA_TYPE**)ctsar_alloc_2d(sizeof(DATA_TYPE),NI,NJ);

    DATA_TYPE **A = (DATA_TYPE**)ctsar_alloc_2d(sizeof(DATA_TYPE),NI,NK);
    DATA_TYPE **B = (DATA_TYPE**)ctsar_alloc_2d(sizeof(DATA_TYPE),NK,NJ);

    /* Initialize array. */
    init(A, B, C, C_outputFromGpu);
    ctsar_pre_init();

    int iters;
    char * env;
    if(env = getenv("TEST_ITERATIONS")){
        iters = atoi(env);
    }else{
        iters = 1;
    }
    t_start = rtclock();

    ctsar_init(&s,NI,CTSAR_START, CTSAR_DEV_CPU | CTSAR_DEV_GPU, NULL,NULL,NULL);
    for(int i=0; i < iters; i++)
    {
        double lt_start, lt_end;
        lt_start = rtclock();
        /* grunCorr(symmat_outputFromGpu, symmat_outputFromGpu, stddev_Gpu, mean_Gpu, float_n, eps); */
        if((env = getenv("OMP_CTSAR_FALLBACK")) && atoi(env)){
            runGemm(A, B, C_outputFromGpu);
        }else{
            ctsar_init(&s,NI,CTSAR_START, CTSAR_DEV_CPU | CTSAR_DEV_GPU, NULL,NULL,NULL);
            runGemm_gpu(A, B, C_outputFromGpu);
        }
        lt_end = rtclock();
        fprintf(stderr, "GPU Runtime:it%d: %0.6lfs\n", i, lt_end - lt_start);

    }


    t_end = rtclock();
    fprintf(stderr, "GPU Runtime: %0.6lfs\n", t_end - t_start);


    if((env = getenv("TEST_VERIFY")) && atoi(env)){
        t_start = rtclock();

        runGemm(A, B, C);

        t_end = rtclock();
        fprintf(stderr, "CPU Runtime: %0.6lfs\n", t_end - t_start);

        compareResults(C, C_outputFromGpu);
    }

    return 0;
}
