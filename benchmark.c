#include <stdio.h>
#include <omp.h>

#define nx 10000000
#define N 100
#define NLOOP 100

double u_normal[nx],un_normal[nx];
double u_naive[nx],un_naive[nx];
double u_high[nx],un_high[nx];

void assert(int b);

void normal(double *u, double *un);
void naive(double *u, double *un);
void high(double *u, double *un);

void equality_test(double *u1, double *u2)
{
    int i;
    for (i=0; i<nx; i++) {
        assert(u1[i]==u2[i]);
    }
}

int main(void){
    int it,i;
    double start_time, end_time;

    // === normal ===
    // ファーストタッチ&初期化
    for (i=0; i<nx; i++){
        u_normal[i]=un_normal[i]=0.0;
    }
    u_normal[0]=un_normal[0]=0.5;
    u_normal[nx-1]=un_normal[nx-1]=1.0;

    printf("[+] case normal\n\n");
    start_time = omp_get_wtime();
    normal(u_normal, un_normal);
    end_time = omp_get_wtime();
    printf("elapsed time: %lf\n", end_time - start_time);

    // === naive ===
    // ファーストタッチ&初期化
    for (i=0; i<nx; i++){
        u_naive[i]=un_naive[i]=0.0;
    }
    u_naive[0]=un_naive[0]=0.5;
    u_naive[nx-1]=un_naive[nx-1]=1.0;

    printf("[+] case naive\n\n");
    start_time = omp_get_wtime();
    naive(u_naive, un_naive);
    end_time = omp_get_wtime();
    printf("elapsed time: %lf\n", end_time - start_time);

    // === high ===
    // ファーストタッチ&初期化
    for (i=0; i<nx; i++){
        u_high[i]=un_high[i]=0.0;
    }
    u_high[0]=un_high[0]=0.5;
    u_high[nx-1]=un_high[nx-1]=1.0;

    printf("[+] case high\n\n");
    start_time = omp_get_wtime();
    high(u_high, un_high);
    end_time = omp_get_wtime();
    printf("elapsed time: %lf\n", end_time - start_time);

    // === equality test ===
    equality_test(u_normal, u_naive);
    equality_test(u_normal, u_high);

    return 0;
}
