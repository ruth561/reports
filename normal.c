// 通常の実装

#include <stdio.h>
#include <omp.h>

#include "testcase.h"


void normal(double *u, double *un){
    int it,i;

    for (it=0; it<NLOOP; it++){
        for (i=1; i<nx-1; i++){
            un[i]=u[i]*2.0-u[i+1]-u[i-1];
        }
        for (i=1; i<nx-1; i++){
            u[i]=un[i];
        }
    }
}
