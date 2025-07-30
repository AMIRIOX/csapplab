#if 0
Warning: These are incorrect ways of thinking!
For a solution, see bits.c
#endif

#include <stdio.h>

int isTmax(int x) { return !((~x + ~0) ^ x) & !!(x ^ ~0); }


int allOddBits(int x) { 
    int mask = 0xAA;
    // 1010 1010
    mask |= (mask << 8);
    // 1010 1010 1010 1010
    mask |= (mask << 16);
    return !((x & mask) ^ x);
}

int isAsciiDigit(int x) {
    int zero = 0x30; // 0b 110000
                     // 0b 110001
                     // 0b 110010
                     // 0b 110011
                     // 0b 110100
                     // 0b 110101
                     // 0b 110110
                     // 0b 110111
                     // 0b 101000
    int nine = 0x39; // 0b 111001
                     // 0b 11****
    return (!((x >> 4) ^ 3)) & (!((x >> 3) & 1) | ((x & 0xF) ^ 9) );
}

int conditional(int x, int y, int z) {
    int xp = ~(!!x - 1);   // =0000 if x=0000; =1111 if x has 1;
    return (y & xp) | (z & ~xp);
}

int logicalNeg(int x) {
    int lowbit = x & (~x + 1); // 0 if x = 0; 001000 if x = 001***
    return ((lowbit + ~0) >> 31) + 1;
}

int isLessOrEqual(int x, int y) {
    // +overflow?
    int d = x + ~y + 1;
    return !!(d & (1 << 31)) | !d;
}

int howManyBits(int x) {
    int lowbit = x & (~x + 1);
    int r = 0;
    r |= (lowbit & 0xAAAAAAAA) << 0; // r odd; 1010
    r |= (lowbit & 0xCCCCCCCC) << 1; // r = 2/3, 6/7, 10/11; 1100
    r |= (lowbit & 0xF0F0F0F0) << 2; // r = 4/5/6/7; 1111_0000
    r |= (lowbit & 0xFF00FF00) << 3; // r = 8~15; 1111_1111_0000_0000
    r |= (lowbit & 0xFFFF0000) << 4; // 找规律
    return r + (!!(x ^ 0) & !!(x ^ ~0));
}

signed main() {
    int a, b, c;
    while (scanf("%d", &a) == 1) {
        printf("%d\n", howManyBits(a));
    }
    return 0;
}
