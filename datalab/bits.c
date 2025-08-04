/*
 * CS:APP Data Lab
 *
 * <amiriox>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) { return ~(~(~x & y) & ~(x & ~y)); }
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) { return 1 << 31; }
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */

// x == 0b011111....
//  -1== 0b111111....
//~x == 0b100000....
// !(x ^ ~0) == 1 only when x == -1
// UB!
int isTmax(int x) { return !(~(x + 1) ^ x) & !!(x ^ ~0); }
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
// 1010 1010
// 1101 0101
// 0111 1111
// 1000 0000
int allOddBits(int x) {
    int mask = 0xAA;
    // 1010 1010
    mask |= (mask << 8);
    // 1010 1010 1010 1010
    mask |= (mask << 16);
    return !((x | mask) ^ x);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { return ~x + 1; }
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    int zero = 0x30; // 0b 110000
    int nine = 0x39; // 0b 111001
                     // 0b 11****
                     //      0110
    return (!((x >> 4) ^ 3)) & (!((x >> 3) & 1) | !(x & 6));
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    int xp = ~(!!x + ~0); // =0000 if x=0000; =1111 if x has 1;
                          // replace `-1` with `+ ~0`
    return (y & xp) | (z & ~xp);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int signx = !!(x & (1 << 31));
    int signy = !!(y & (1 << 31));
    int diff = (signx ^ signy);
    int d = x + ~y + 1;
    return (diff & (signx & !signy)) | (~diff & (!!(d & (1 << 31)) | !d));
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x) {
    int lowbit = x & (~x + 1); // 0 if x = 0; 001000 if x = 001***
    int xp = ((lowbit + ~0) >> 31) + 1;
    return ~xp + 1 + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90 (?)
 *  Rating: 4
 */
// (log2(highbit(x)) + 1) + 1
// 1 for x == 0 or x == -1  (rule invalid)
// 2 for x == 1             (rule ok)
int howManyBits(int x) {
    // The prohibition of using large constants is
    // the most disgusting thing about this disgusting lab.
    // It forced me to abandon the more elegant masking method
    // and use binary division like enumeration.

    /* shit, i almost forgot macros are also banned.
#ifdef ENABLE_LARGE_CONSTANTS
    // int lowbit = x & (~x + 1);
    // replace lowbit with highbit
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x = (x + 1) >> 1;
    int r = 0;
    r |= !!(x & 0xAAAAAAAA) << 0; // r odd; 1010
    r |= !!(x & 0xCCCCCCCC) << 1; // r = 2/3, 6/7, 10/11; 1100
    r |= !!(x & 0xF0F0F0F0) << 2; // r = 4/5/6/7; 1111_0000
    r |= !!(x & 0xFF00FF00) << 3; // r = 8~15; 1111_1111_0000_0000
    r |= !!(x & 0xFFFF0000) << 4; // pattern
    return r + (!!(x ^ 0) & !!(x ^ ~0));
#endif
    */
    int sign = x >> 31;
    int y = x ^ sign;
    int r = 0;
    int shift2 = 0, shift4 = 0, shift8 = 0;

    r += !!(y >> 16) << 4;
    y >>= r;

    shift8 = !!(y >> 8) << 3;
    r += shift8;
    y >>= shift8;

    shift4 = !!(y >> 4) << 2;
    r += shift4;
    y >>= shift4;

    shift2 = !!(y >> 2) << 1;
    r += shift2;
    y >>= shift2;

    r += (y >> 1) + 1;

    return r + (!!(x ^ 0) & !!(x ^ ~0));
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    //   1 | 8 | 23
    // sign exp frac
    unsigned sign = (uf >> 31);
    unsigned exp = (uf << 1) >> (24);
    unsigned frac = (uf << 9) >> 9;
    unsigned dt = 1;
    if (!(uf << 1))
        return uf;
    if (exp == 0xFF) /*if(!frac)*/
        return uf;
    if (!exp) {
        if (!(frac >> 22)) {
            dt--;
        }
        frac <<= 1;
    }
    return (sign << 31) | ((exp + dt) << 23) | frac;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    //   1 | 8 | 23
    // sign exp frac
    unsigned sign = (uf >> 31);
    unsigned exp = (uf << 1) >> (24);
    unsigned frac = (uf << 9) >> 9;
    unsigned E = 0, A = 0;
    if (!(uf << 1) || /*!exp*/ exp < 0x7f)
        return 0;

    E = exp - 0x7f;
    A = (1U << 23) | frac;
    // check overflow
    // INT_MAX = T32Max = 0b_0111_1111_1111_1111_1111_1111_1111_1111
    if ((E >= 31 && (!sign || (sign && frac))) || exp == 0xFF) {
        return 0x80000000u;
    }
    if (E >= 23) {
        A <<= (E - 23);
    } else {
        A >>= (23 - E);
    }

    if (sign) {
        return ~A + 1;
    } else {
        return A;
    }
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    // (-1)^s * M * 2^{e-bias}
    // 先试试只用 e-0x7f 表示
    // exp 8 位无符号整型
    // 0b11111111(无穷大/Nan) = 2^8-1,
    // 0b11111110 = 2^8-2 = 254
    // U8Min = 0(非规格化), 或 1
    // 规格化数可表示的 2^x x = {e - 127} 取值范围: -126 ~ 127 连续 (保持 M 为 0.0, 即 1.0)
    // 非规格化数: x < 0, M * 2^{-126}, 2^{-1-126}+2^{-2-126}+2^{-3-126}+...+2^{-23-126}
    // 取值范围 2^{-149} 到 2^{-127}

    // 等等, 怎么这代码他妈把评测脚本炸掉了 ???

    unsigned exp = 0, s = 0;

    if (x > 127) {
        return 0x7f800000; //(0xFF << 23);
    } else if (x >= -126 && x <= 127) {
        exp = (127 + x);
        return (exp << 23);
    }
    else if (x >= -149 && x < -126) {
        s = 149 + x;
        return (1U << s);
    }
    /*
    else if (x >= -149 && x < -126) {
        // 23 - (-(x + 126)) == 23 + x + 126
        return (1 << (149 + x));
    }*/
    /*
    else {
        return 0;
    }
    */
    return 0;
    // ? ? ?
}
