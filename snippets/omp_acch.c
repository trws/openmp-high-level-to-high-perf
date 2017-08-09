#pragma acc_region_loop                  \
        acc_copyin(in1[0:end],in2[0:end])\
        acc_copyout(out[0:end])          \
        acc_copy(pow[0:end])             \
        hetero(<cond>,                   \
               <iterations for CPU>)
for (i=0; i<end; i++){
    out[i] = in1[i] * in2[i];
    pow[i] = pow[i]*pow[i];
}

