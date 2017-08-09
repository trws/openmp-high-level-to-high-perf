#pragma acc region for                    \
        copyin(in1[0:end],in2[0:end])     \
        copyout(out[0:end])               \
        copy(pow[0:end])                  \
        hetero(end > 500,adaptive)        \
        partial(in1,\
                in2,\
                out,\
                pow)
for (i=0; i<end; i++){
    out[i] = in1[i]*in2[i];
    pow[i] = pow[i]*pow[i];
}
