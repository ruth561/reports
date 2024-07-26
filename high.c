// ダイヤモンドタイリングによる並列化

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "testcase.h"


int max(int x, int y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

int min(int x, int y) {
    if (x > y) {
        return y;
    } else {
        return x;
    }
}

void assert(int b)
{
    if (!b) {
        fprintf(stderr, "assert!\n");
        exit(-1);
    }
}

// c: center (u[i])
// l: left   (u[i-1])
// r: right  (u[i+1])
double tarnop(double c, double l, double r)
{
    return c*2.0-r-l;
    // return c+l+r;
}

void swap(double *x, double *y)
{
    double tmp = *x;
    *x = *y;
    *y = tmp;
}

void swap_ptr(double **p, double **q)
{
    double *tmp = *p;
    *p = *q;
    *q = tmp;
}

// 最初の"山"を作る処理（両端の処理ではない）
// kは偶数
// uに初期値が入っていて、この関数が終了するときには
// unに山肌の値が、uにその1層下の値が格納されている
void init_diamond_mid(double *u, double *un, int k)
{
    assert(k % 2 == 0);

    int i, j, h;
    // 以下のように山を作成する
    // 
    // *: unの要素
    // +: uの要素
    //
    //     ,    ,    , [+], [+],    ,    ,    ,
    //     ,    , [*], [*], [*], [*],    ,    ,
    //     , [+], [+],  + ,  + , [+], [+],    ,
    //  [*], [*],  * ,  * ,  * ,  * , [*], [*],
    // kN-N           kN-1  kN          (k+1)N-1
    for (h=0; h<N; h++) {
        if (h%2==0) {
            for (i=max(1, k*N-N+h); i<min(nx-1, k*N+N-h); i++) {
                un[i]=tarnop(u[i], u[i-1], u[i+1]);
            }
        } else {
            for (i=max(1, k*N-N+h); i<min(nx-1, k*N+N-h); i++) {
                u[i]=tarnop(un[i], un[i-1], un[i+1]);
            }
        }
    }
    // unとuの偶数高さの要素を入れ替える
    // 
    // *: unの要素
    // +: uの要素
    //
    //     ,    ,    , [*], [*],    ,    ,    ,
    //     ,    , [*], [+], [+], [*],    ,    ,
    //     , [*], [+],  + ,  + , [+], [*],    ,
    //  [*], [+],  * ,  * ,  * ,  * , [+], [*],
    // kN-N           kN-1  kN          (k+1)N-1
    for (h=0; h<N; h++) {
        if (h%2==1) {
            if (k>0) {
                swap(&u[k*N-N+h], &un[k*N-N+h]);
            }
            if (k*N<nx) {
                swap(&u[k*N+N-h-1], &un[k*N+N-h-1]);
            }
        }
    }
}

// it: 何回目のgrowか？
// k: どこをgrowするか？
void grow_diamond(double *u, double *un, int it, int k)
{
    int i, h;
    int begin, end;
    // 最上層と次の層をそれぞれ表すポインタ
    double *top, *second;

    if (it%2==0) {
        top = un;
        second = u;
    } else {
        top = u;
        second = un;
    }

    // ダイヤモンドによって構築される層の枚数は2*N-1
    // 最下層のみ少し特殊になっていて、それぞれのループボディのelse ifの
    // 部分で特別なチェックが行われている。
    for (h=1; h<2*N; h++) {
        // 谷の右側を構築していく
        begin = max(1, k*N);
        end = min(nx-1, (k+1)*N-h/2);
        for (i=begin; i<end; i++) {
            if (i==begin) {
                second[i]=tarnop(top[i], top[i-1],  second[i+1]);
            } else if (i==(k+1)*N-1) {
                second[i]=tarnop(top[i],  second[i-1], top[i+1]);
            } else {
                second[i]=tarnop(top[i],  second[i-1],  second[i+1]);
            }
        }
        // 谷の左側を構築していく
        begin = min(nx-2, k*N-1);
        end = max(1, k*N-N+h/2);
        for (i=begin; end<=i; i--) {
            if (i==begin) {
                second[i]=tarnop(top[i],  second[i-1], top[i+1]);
            } else if (i==k*N-N) {
                second[i]=tarnop(top[i], top[i-1],  second[i+1]);
            } else {
                second[i]=tarnop(top[i],  second[i-1],  second[i+1]);
            }
        }
        swap_ptr(&top, &second);
    }
}

// 最後に凸凹をならす処理
// diamond_growと処理は似ているが、ダイヤモンドを作るのではなく、凸凹を平らにする
void fini_diamond_mid(double *u, double *un, int it, int k)
{
    int i, h;
    int begin, end;
    // 最上層と次の層をそれぞれ表すポインタ
    double *top, *second;

    if (it%2==0) {
        top = un;
        second = u;
    } else {
        top = u;
        second = un;
    }

    for (h=1; h<N; h++) {
        // 谷の右側を構築していく
        begin = max(1, k*N);
        end = min(nx-1, (k+1)*N-h);
        for (i=begin; i<end; i++) {
            if (i==begin) {
                second[i]=tarnop(top[i], top[i-1],  second[i+1]);
            } else if (i==(k+1)*N-1) {
                second[i]=tarnop(top[i],  second[i-1], top[i+1]);
            } else {
                second[i]=tarnop(top[i],  second[i-1],  second[i+1]);
            }
        }
        // 谷の左側を構築していく
        begin = min(nx-2, k*N-1);
        end = max(1, k*N-N+h);
        for (i=begin; end<=i; i--) {
            if (i==begin) {
                second[i]=tarnop(top[i],  second[i-1], top[i+1]);
            } else if (i==k*N-N) {
                second[i]=tarnop(top[i], top[i-1],  second[i+1]);
            } else {
                second[i]=tarnop(top[i],  second[i-1],  second[i+1]);
            }
        }
        swap_ptr(&top, &second);
    }

    // 谷の左側を構築していく
    begin = max(1, k*N-N+1);
    end = k*N;
    for (i=begin; i<end; i+=2) {
        swap(&top[i], &second[i]);
    }

    begin = min(nx-2, (k+1)*N-2);
    end = max(0, k*N);
    for (i=begin; i>=end; i-=2) {
        swap(&top[i], &second[i]);
    }

}

void high(double *u, double *un){
    int it,i,k,h;

    // nxが2*Nの倍数となるようにNを設定する
    // 山の初期化は2N→4N→...を中心に行っていく
    #pragma omp parallel for
    for (k=0; k<=nx/N; k+=2) {
        init_diamond_mid(u, un, k);
    }

    // itと層の関係性としては、(it+1)*N目の計算をできるようにする
    for (it=0; (it+1)*N<NLOOP; it++) {
        if (it%2==0) {
            #pragma omp parallel for
            for (k=1; k<=nx/N; k+=2) {
                grow_diamond(u, un, it, k);
            }
        } else {
            #pragma omp parallel for
            for (k=0; k<=nx/N; k+=2) {
                grow_diamond(u, un, it, k);
            }
        }
    }

    if (it%2==0) {
        #pragma omp parallel for
        for (k=1; k<=nx/N; k+=2) {
            fini_diamond_mid(u, un, it, k);
        }
        #pragma omp parallel for
        for (i=0; i<nx; i++) {
            u[i]=un[i];
        }
    } else {
        #pragma omp parallel for
        for (k=0; k<=nx/N; k+=2) {
            fini_diamond_mid(u, un, it, k);
        }
    }
}
