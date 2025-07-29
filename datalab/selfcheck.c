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


signed main() {
    int a, b, c;
    while (scanf("%d %d", &a, &b) == 2) {
        printf("%d\n", isLessOrEqual(a, b));
    }
    return 0;
}
