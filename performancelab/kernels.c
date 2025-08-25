/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include "defs.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Please fill in the following team struct
 */
team_t team = {
    "Rhodes Island", /* Team name */

    "Dr.amiriox",              /* First member full name */
    "wumingyun2120@gmail.com", /* First member email address */

    "1", /* Second member full name (leave blank if none) */
    "1"  /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
inline static int min(int a, int b);
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) {
    int i, j, p, q;
#define B (32)
    // 按 32KB L1-d 计算, sqrt(30000 / 2 / sizeof(unsigned short)) = 50
    // 测试接近的 32, 50, 64 后发现 32 表现最好

    for (p = 0; p < dim; p += B) {
        for (q = 0; q < dim; q += B) {
            for (i = p; i < min(dim, p + B); i++) {
                for (j = q; j < min(dim, q + B); j++) {
                    dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
                }
                // 很不幸, 这里使用循环展开降低了分数, 没有任何意义
                // 可能是爆寄存器, 可能是 RIDX 宏的乘法开销
                /*
                dst[RIDX(dim - 1 - (q + 0), i, dim)] = src[RIDX(i, q + 0, dim)];
                dst[RIDX(dim - 1 - (q + 1), i, dim)] = src[RIDX(i, q + 1, dim)];
                dst[RIDX(dim - 1 - (q + 2), i, dim)] = src[RIDX(i, q + 2, dim)];
                dst[RIDX(dim - 1 - (q + 3), i, dim)] = src[RIDX(i, q + 3, dim)];
                dst[RIDX(dim - 1 - (q + 4), i, dim)] = src[RIDX(i, q + 4, dim)];
                dst[RIDX(dim - 1 - (q + 5), i, dim)] = src[RIDX(i, q + 5, dim)];
                dst[RIDX(dim - 1 - (q + 6), i, dim)] = src[RIDX(i, q + 6, dim)];
                dst[RIDX(dim - 1 - (q + 7), i, dim)] = src[RIDX(i, q + 7, dim)];
                dst[RIDX(dim - 1 - (q + 8), i, dim)] = src[RIDX(i, q + 8, dim)];
                dst[RIDX(dim - 1 - (q + 9), i, dim)] = src[RIDX(i, q + 9, dim)];
                dst[RIDX(dim - 1 - (q + 10), i, dim)] = src[RIDX(i, q + 10, dim)];
                dst[RIDX(dim - 1 - (q + 11), i, dim)] = src[RIDX(i, q + 11, dim)];
                dst[RIDX(dim - 1 - (q + 12), i, dim)] = src[RIDX(i, q + 12, dim)];
                dst[RIDX(dim - 1 - (q + 13), i, dim)] = src[RIDX(i, q + 13, dim)];
                dst[RIDX(dim - 1 - (q + 14), i, dim)] = src[RIDX(i, q + 14, dim)];
                dst[RIDX(dim - 1 - (q + 15), i, dim)] = src[RIDX(i, q + 15, dim)];
                */
                /*
                dst[RIDX(dim - 1 - (q + 16), i, dim)] = src[RIDX(i, q + 16, dim)];
                dst[RIDX(dim - 1 - (q + 17), i, dim)] = src[RIDX(i, q + 17, dim)];
                dst[RIDX(dim - 1 - (q + 18), i, dim)] = src[RIDX(i, q + 18, dim)];
                dst[RIDX(dim - 1 - (q + 19), i, dim)] = src[RIDX(i, q + 19, dim)];
                dst[RIDX(dim - 1 - (q + 20), i, dim)] = src[RIDX(i, q + 20, dim)];
                dst[RIDX(dim - 1 - (q + 21), i, dim)] = src[RIDX(i, q + 21, dim)];
                dst[RIDX(dim - 1 - (q + 22), i, dim)] = src[RIDX(i, q + 22, dim)];
                dst[RIDX(dim - 1 - (q + 23), i, dim)] = src[RIDX(i, q + 23, dim)];
                dst[RIDX(dim - 1 - (q + 24), i, dim)] = src[RIDX(i, q + 24, dim)];
                dst[RIDX(dim - 1 - (q + 25), i, dim)] = src[RIDX(i, q + 25, dim)];
                dst[RIDX(dim - 1 - (q + 26), i, dim)] = src[RIDX(i, q + 26, dim)];
                dst[RIDX(dim - 1 - (q + 27), i, dim)] = src[RIDX(i, q + 27, dim)];
                dst[RIDX(dim - 1 - (q + 28), i, dim)] = src[RIDX(i, q + 28, dim)];
                dst[RIDX(dim - 1 - (q + 29), i, dim)] = src[RIDX(i, q + 29, dim)];
                dst[RIDX(dim - 1 - (q + 30), i, dim)] = src[RIDX(i, q + 30, dim)];
                dst[RIDX(dim - 1 - (q + 31), i, dim)] = src[RIDX(i, q + 31, dim)];
                */
            }
        }
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions() {
    // add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
}

/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
inline static int min(int a, int b) { return (a < b ? a : b); }
inline static int max(int a, int b) { return (a > b ? a : b); }

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
inline static void initialize_pixel_sum(pixel_sum *sum) {
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
inline static void accumulate_sum(pixel_sum *sum, pixel p) {
    sum->red += (int)p.red;
    sum->green += (int)p.green;
    sum->blue += (int)p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
inline static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) {
    current_pixel->red = (unsigned short)(sum.red / sum.num);
    current_pixel->green = (unsigned short)(sum.green / sum.num);
    current_pixel->blue = (unsigned short)(sum.blue / sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
inline static pixel avg(int dim, int i, int j, pixel *src) {
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++) {
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++) {
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);
        }
    }

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++) {
        size_t ridx = RIDX(i, 0, dim);
        for (j = 0; j < dim; j++, ridx++) {
            dst[ridx] = avg(dim, i, j, src);
        }
    }
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    // add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}
