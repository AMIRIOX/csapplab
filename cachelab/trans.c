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
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
#define BSIZE 8
    int a, b, c, d, e, f, g, h;
    printf("%s\n", transpose_submit_desc);
    for (int kk = 0; kk < N; kk += BSIZE) {
        for (int jj = 0; jj < M; jj += BSIZE) {
            // a chunk with left-top corner (kk, jj)

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
            } else {
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
            }
        }
    }
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
