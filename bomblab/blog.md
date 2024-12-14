
---

Border relations with Canada have never been better.
直接读phase_1 的 $rdi.

--- 

1 2 4 8 16 32
利用断点跳到 read_six_number 后, 发现核心代码 add $eax $eax

---

5 206
rsp+8 rsp+12分别是你输入的两个数, int四字节. 发现一堆, mov jmp里只有第五个的差能整除8并且mov的值是206(最后几行是核心代码 cmp 0xc(%rsp), %eax

---

7 0
打断点到 phase_4(), 阅读汇编得: 
输入两个数, 输入的第一个数 <= 14
rdi: 输入的第一个数
rsi: 0
rdx: 14
func4(x1, 0, 14, 0);

进入 func4() 继续阅读汇编, 顺便翻译一下: 
def func4(rdi, rsi, rdx, rcx): 
    rax = rdx - rsi
    rcx = rax shr 31
    rax += rcx
    rax sar= 1
    rcx = rax + rsi

    // summary:
    // rax = ((rdx - rsi) + (rdx - rsi) shr 31) sar 1
    // rcx = ((rdx - rsi) + (rdx - rsi) shr 31) sar 1 + rsi
    if (rdi <= rcx) 
        // 跳到ff2
        if(rdi >= rcx)
	      return 0;
        else
            rax = func4(rdi, rsi = rcx + 1, rdx, rcx)
            return (rax = 2 * rax + 1)
    else 
        rax = fun4(rdi, rsi, rdx = rcx - 1, rcx)
	   return 2 * rax

if(func4() == 0 && ($rsp + 8) == 0) {
   ok
} else bomb!

优化后得:
f (x1, $2, $3): 
    x1 <= $2 + ($3 - $2)/2
        x1 == $2 + ($3 - $2)/2: return 0
        else: return 2 * f(x1, ($3 - $2)/2 + 1, $3) + 1
    else:
        return 2 * f(x1, $2, $2 + ($3 - $2)/2 - 1)
10 0 14
return 2 * f(10, 0, 6)

然后发现其实令 x1 = $2 + ($3 - $2) / 2 即 0 + (14 - 2) / 2 = 7 就行了

---

y_^EFG
字符串长度为6
rbx = rdi(input)
rax = 0
while(rax < 6) {
    rdx = (*(rbx + rax) & 0xf) + 0x4024b0   // input[i] & 1111 + 0x4024b0
    *(rsp + 10 + rax) = rdx                   // input[i] = 
    rax += 1                                // i++;
}
*(rsp + 16) = 0  // '\0'
rdi = rsp + 10    // input after processing
rsi = $0x40245e   // "flyers"
strings_not_equal(rdi, rsi)

           11
maduiersnfotvbyl
0123456789
          10

@: 0x4024b0
f: 0x4024b9  +9     0x0009
l: 0x4024bf  +15    0x000f
y: 0x4024be  +14    0x000e
e: 0x4024b5  +5     0x0005
r: 0x4024b6  +6     0x0006
s: 0x4024b7  +7     0x0007
+dd

input[i] ASCII 取后四位 + 0x4024b0
令一个字符c的ASCII的十六进制后四位分别为以上的
%rdx 64位, %edx 32位, $dl 8位
p /x $dl 如输出 0x61, 6是四位, 1也是四位, 因为十六进制最大f(15, 1111)
所以只要ASCII 是 `0x*9`, `0x*f`, `0x*e`, `0x*5`, `0x*6`, `0x*7` 的字符就可以了
这里随便选的y_^EFG

---


r13 =  <+8>rsp  (init - 50)
rsi = <+8>rsp

下面是废的, 我有空再补.

这个难度很大.
总之是先查6个数都<=6 且不能重复

然后将每个数 x 变为 7 - x

然后根据新的xi安排在0x6032d0处的一个链表, 安排完要求顺序值递减
查看一下这个链表:
(gdb) x/24w 0x6032d0
0x6032d0 <node1>:       332     1       6304480 0
0x6032e0 <node2>:       168     2       6304496 0
0x6032f0 <node3>:       924     3       6304512 0
0x603300 <node4>:       691     4       6304528 0
0x603310 <node5>:       477     5       6304544 0
0x603320 <node6>:       443     6       0       0

递减的话就是 3 4 5 6 1 2
根据7映射回去就是 4 3 2 1 6 5

读入6个数字(rsp也变了)

r14 = <+23>rsp
r12d = 0
rbp = r13
rax = *r13 - 1
if(rax >= 5) {
    //跳到52
    r12d += 1
    if(r12d == 6) {
        // 跳到 95
        rsi = *(rsp + 0x18)
        rax = r14
        rcx = 0x7
		
<+118>
        while(rax != rsi) {
<+108>
            rdx = rcx
            rdx -= *rax
            *rax = rdx
            rax += 0x4 
        }

<+123>
        rsi = 0

<+166>
while(rcx >= 1) {
<+163>
        rcx = *(rsp + rsi)
        while(rcx >= 0x1) {
            // 跳到<+143>
<+143>
                     rdx = $0x6032d0
<+148>
            *(0x20 + rsp + 2 * rsi) = rdx
<+153>
            rsi += 0x4
            if(rsi = 0x18) {
                // 跳到183
<+183>
                codes here (201)
<+212>          
                while(rax != rsi) {
                
                }else {

                }
                if(rax == rsi) {
                     //跳到222
<+222>               
                     codes here
                }else {
<+217>
                     codes here
<+220>
                     直接跳201了, 我不理解
                }
            }else {
<+163>
                rcx = *(rsp + rsi)
            }
        }
}

        rax = 0x1
        rdx = 0x6032d0
        // 跳到 + 130
<+130>
	  do {
                   rdx = *(rdx + 0x8)
           rax += 1
        }while(rax != rcx);
        // 跳到<+148>
        goto <+148>:
        goto <+143>:
        
    }
}else bomb!
