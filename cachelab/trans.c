/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
int min(int a, int b) { return a <= b ? a : b; }
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    /*
     * 32x32: 4*32=128, set_id=0b100=4, 不会自我冲突
     * */
    if (N != 64) {
#define BSIZE 8
        int a, b, c, d, e, f, g, h;
        for (int kk = 0; kk < N; kk += BSIZE) {
            for (int jj = 0; jj < M; jj += BSIZE) {
                // a chunk with left-top corner (kk, jj)
                for (int i = jj; i < min(jj + BSIZE, M); i++) {
                    // 试图通过循环展开分离 读A 和 写B,
                    // 从而防止 A 和 B 中的元素地址映射到统一缓存行造成冲突不命中
                    // 还要注意 A 自己本身由于访问模式也可能映射到统一缓存行:(
                    // B 是连续的良好访问模式, 应该没什么问题

                    // 还有一个问题是 a = A[0][0] 和 B[0][0] = a 这样的对角线问题
                    // kk == jj 时, 例如对与内循环第一次迭代 i = jj
                    // 下面读 A 的一列的第一个 if 就会变成 a = A[kk][jj] 也就是相等的 A[x][x]
                    // 下面写 B 的一行的第一个 if 就会变成 B[jj][jj] = a 同上
                    // A[x][x] 和 B[x][x] 的地址大概率映射在同一组上,
                    // 而对于 E = 1 的直接映射缓存, 只有一个缓存行
                    // 所以 A 的第 x 行会和 B 的第 x 行冲突在统一缓存行
                    // B 会因为冲突残忍地把 A 这一整行踢出缓存行
                    // 内循环第二次迭代 i = jj + 1 时, A[kk] 这一整行的缓存就没有了
                    // 但是得益于循环展开带来的读写分离,
                    // A 这一行的缓存每次大循环只会 miss 一次
                    // 如果不循环展开, 写完整的循环,
                    // 小循环每次迭代, A, B 都会反复争抢一个缓存行
                    // 非对角线块就不会, 因为读写的缓存行没有交集, 不会冲突

                    if (kk + 0 < N)
                        a = A[kk][i];
                    if (kk + 1 < N)
                        b = A[kk + 1][i];
                    if (kk + 2 < N)
                        c = A[kk + 2][i];
                    if (kk + 3 < N)
                        d = A[kk + 3][i];
                    if (kk + 4 < N)
                        e = A[kk + 4][i];
                    if (kk + 5 < N)
                        f = A[kk + 5][i];
                    if (kk + 6 < N)
                        g = A[kk + 6][i];
                    if (kk + 7 < N)
                        h = A[kk + 7][i];

                    if (kk + 0 < N)
                        B[i][kk] = a;
                    if (kk + 1 < N)
                        B[i][kk + 1] = b;
                    if (kk + 2 < N)
                        B[i][kk + 2] = c;
                    if (kk + 3 < N)
                        B[i][kk + 3] = d;
                    if (kk + 4 < N)
                        B[i][kk + 4] = e;
                    if (kk + 5 < N)
                        B[i][kk + 5] = f;
                    if (kk + 6 < N)
                        B[i][kk + 6] = g;
                    if (kk + 7 < N)
                        B[i][kk + 7] = h;
                }
            }
        }
#undef BSIZE
    } else {
        // 64 * 64 的矩阵, A[i] 和 A[i+4] 会在同一组导致冲突
        // !!! 但是 BSIZE = 4 的方案使得一个缓存行(8个整数)只能有效利用一半(4个)
        // 所以还是要考虑 BSIZE = 8 的方案
#define BSIZE 8
        int a, b, c, d, e, f, g, h;
        for (int kk = 0; kk < N; kk += BSIZE) {
            for (int jj = 0; jj < M; jj += BSIZE) {
                // 还是 8 * 8, 但是要对每一分块再分块

                // 我操, 写错了, A 的 (kk, jj) 块要转置到 B 的 (jj, kk) 块
                // OK 改好了

                // 先复制前四行, 因为第五行会出现 A[0] 和 A[4] 冲突
                for (int i = 0; i < 4; i++) {
                    a = A[kk + i][jj + 0];
                    b = A[kk + i][jj + 1];
                    c = A[kk + i][jj + 2];
                    d = A[kk + i][jj + 3];
                    e = A[kk + i][jj + 4];
                    f = A[kk + i][jj + 5];
                    g = A[kk + i][jj + 6];
                    h = A[kk + i][jj + 7];

                    B[jj + 0][kk + i] = a;
                    B[jj + 1][kk + i] = b;
                    B[jj + 2][kk + i] = c;
                    B[jj + 3][kk + i] = d;
                    B[jj + 0][kk + 4 + i] = e;
                    B[jj + 1][kk + 4 + i] = f;
                    B[jj + 2][kk + 4 + i] = g;
                    B[jj + 3][kk + 4 + i] = h;
                }
                // A 的前四行, B 的前四行都在缓存里
                // A 的前四行已经没用了, 可以直接踢出
                for (int i = 0; i < 4; i++) {
                    // 缓冲 B 的右上角(此时位置错误)
                    // hit B 第 i 行一行后半的缓存
                    a = B[jj + i][kk + 4 + 0];
                    b = B[jj + i][kk + 4 + 1];
                    c = B[jj + i][kk + 4 + 2];
                    d = B[jj + i][kk + 4 + 3];

                    // A 的左下角转置并正确放置到 B 的右上角
                    // evict A 前四行的缓存, 加载/hit 后四行
                    // hit B 第 i 行一行后半的缓存
                    e = A[kk + 4 + 0][jj + i];
                    f = A[kk + 4 + 1][jj + i];
                    g = A[kk + 4 + 2][jj + i];
                    h = A[kk + 4 + 3][jj + i];
                    B[jj + i][kk + 4] = e;
                    B[jj + i][kk + 4 + 1] = f;
                    B[jj + i][kk + 4 + 2] = g;
                    B[jj + i][kk + 4 + 3] = h;

                    // 此时 B 的第 i 行缓存已经没用了, 可以直接踢出

                    // 恢复 B 的右上角到正确的位置(左下)
                    // evict B 第 i 行的缓存, 加载第 i + 4 行
                    B[jj + i + 4][kk + 0] = a;
                    B[jj + i + 4][kk + 1] = b;
                    B[jj + i + 4][kk + 2] = c;
                    B[jj + i + 4][kk + 3] = d;
                }
                // 还剩 B 的右下角
                for (int i = 0; i < 4; i++) {
                    // A 的下半部分第 i 行
                    a = A[kk + 4 + i][jj + 4 + 0];
                    b = A[kk + 4 + i][jj + 4 + 1];
                    c = A[kk + 4 + i][jj + 4 + 2];
                    d = A[kk + 4 + i][jj + 4 + 3];
                    // B 的下半部分 4 行
                    B[jj + 4 + 0][kk + 4 + i] = a;
                    B[jj + 4 + 1][kk + 4 + i] = b;
                    B[jj + 4 + 2][kk + 4 + i] = c;
                    B[jj + 4 + 3][kk + 4 + i] = d;
                }
                // 妈的终于过了, 我快写死了
            }
        }
#undef BSIZE

#ifdef BSIZE4
#define BSIZE 4
        int a, b, c, d;
        for (int kk = 0; kk < N; kk += BSIZE) {
            for (int jj = 0; jj < M; jj += BSIZE) {
                // a chunk with left-top corner (kk, jj)

                // 我真希望写一些宏魔法
                // 但这里空白太小, 写不下 (迫真

                // 32 * 32 的矩阵在循环展开后每块只冲突一次, 规模小可以接受
                // 64 * 64 仍然比满分要求的 misses 高了 300 多

                if (kk != jj) {
                    for (int i = jj; i < min(jj + BSIZE, M); i++) {
                        if (kk + 0 < N)
                            a = A[kk][i];
                        if (kk + 1 < N)
                            b = A[kk + 1][i];
                        if (kk + 2 < N)
                            c = A[kk + 2][i];
                        if (kk + 3 < N)
                            d = A[kk + 3][i];

                        if (kk + 0 < N)
                            B[i][kk] = a;
                        if (kk + 1 < N)
                            B[i][kk + 1] = b;
                        if (kk + 2 < N)
                            B[i][kk + 2] = c;
                        if (kk + 3 < N)
                            B[i][kk + 3] = d;
                    }
                } else {
                    // 4 * 4 的块内, 左上, 右下的 2 * 2 块对位复制
                    // 错位复制+转置会导致边界非常难以确定, 我脑子烧了
                    // 但好在这是 64 * 64, 应该不会涉及太麻烦的边界问题
                    // 但是错位转置本身就很烧脑了
                    // kk == jj;
                    // 。。。但是我这么写完反而更烂了, 反而低了 0.2 分
                    // 我没招了
                    int e, f, g, h;
                    a = A[kk + 0][jj + 0];
                    b = A[kk + 0][jj + 1];
                    c = A[kk + 0][jj + 2];
                    d = A[kk + 0][jj + 3];
                    e = A[kk + 1][jj + 0];
                    f = A[kk + 1][jj + 1];
                    g = A[kk + 1][jj + 2];
                    h = A[kk + 1][jj + 3];

                    B[jj + 2][kk + 2] = a;
                    B[jj + 3][kk + 2] = b;
                    B[jj + 2][kk + 0] = c;
                    B[jj + 3][kk + 0] = d;
                    B[jj + 2][kk + 3] = e;
                    B[jj + 3][kk + 3] = f;
                    B[jj + 2][kk + 1] = g;
                    B[jj + 3][kk + 1] = h;

                    a = A[kk + 2][jj + 0];
                    b = A[kk + 2][jj + 1];
                    c = A[kk + 2][jj + 2];
                    d = A[kk + 2][jj + 3];
                    e = A[kk + 3][jj + 0];
                    f = A[kk + 3][jj + 1];
                    g = A[kk + 3][jj + 2];
                    h = A[kk + 3][jj + 3];

                    B[jj + 0][kk + 2] = a;
                    B[jj + 1][kk + 2] = b;
                    B[jj + 0][kk + 0] = c;
                    B[jj + 1][kk + 0] = d;
                    B[jj + 0][kk + 3] = e;
                    B[jj + 1][kk + 3] = f;
                    B[jj + 0][kk + 1] = g;
                    B[jj + 1][kk + 1] = h;

                    swap(&B[jj + 0][kk + 0], &B[jj + 2][kk + 2]);
                    swap(&B[jj + 0][kk + 1], &B[jj + 2][kk + 3]);
                    swap(&B[jj + 1][kk + 0], &B[jj + 3][kk + 2]);
                    swap(&B[jj + 1][kk + 1], &B[jj + 3][kk + 3]);

                    // swap(&B[jj + 0][kk + 2], &B[jj + 2][kk + 0]);
                    // swap(&B[jj + 0][kk + 3], &B[jj + 2][kk + 1]);
                    // swap(&B[jj + 1][kk + 2], &B[jj + 3][kk + 0]);
                    // swap(&B[jj + 1][kk + 3], &B[jj + 3][kk + 1]);
                }
            }
        }
#undef BSIZE
#endif
    }
#ifdef DIG
    else {
        // 当块位于整体的对角线上即 kk = jj 时,
        // 要从 A[x][x] 读内存写到 B[x][x]
        // TODO: ?

        for (int i = kk; i < kk + BSIZE; i++) {
            int tmp = 0;
            int tmparg = 0;
            for (int j = jj; j < jj + BSIZE; j++) {
                if (i == j) {
                    tmp = A[i][i];
                    tmparg = i;
                } else {
                    B[j][i] = A[i][j];
                }
            }
            B[tmparg][tmparg] = tmp;
        }

        /*
        for (int i = kk; i < min(kk + BSIZE, N); i++) {
            if (jj + 0 < M)
                a = A[i][jj + 0];
            if (jj + 1 < M)
                b = A[i][jj + 1];
            if (jj + 2 < M)
                c = A[i][jj + 2];
            if (jj + 3 < M)
                d = A[i][jj + 3];
            if (jj + 4 < M)
                e = A[i][jj + 4];
            if (jj + 5 < M)
                f = A[i][jj + 5];
            if (jj + 6 < M)
                g = A[i][jj + 6];
            if (jj + 7 < M)
                h = A[i][jj + 7];

            if (jj + 0 < N)
                B[i][jj + 0] = a;
            if (jj + 1 < N)
                B[i][jj + 1] = b;
            if (jj + 2 < N)
                B[i][jj + 2] = c;
            if (jj + 3 < N)
                B[i][jj + 3] = d;
            if (jj + 4 < N)
                B[i][jj + 4] = e;
            if (jj + 5 < N)
                B[i][jj + 5] = f;
            if (jj + 6 < N)
                B[i][jj + 6] = g;
            if (jj + 7 < N)
                B[i][jj + 7] = h;
        }

        for (int i = kk; i < min(kk + BSIZE, M); i++) {
            for (int j = i + 1; j < min(jj + BSIZE, N); j++) {
                a = B[i][j];
                B[i][j] = B[j][i];
                B[j][i] = a;
            }
        }
        */
    }
#endif
}
/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
