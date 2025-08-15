#include "cachelab.h"
#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_HEX_LEN 64

static int hit, miss, evic; // 我不管了我就要用全局变量
static int tick;            // 这也是一个蠢实现, 虽然高速缓存基本上是硬件里的逻辑, 但即使是模拟,
                            // 如果有多线程也会有并发问题, 替换次数太多溢出问题...

// 哎我操我的代码怎么这么烂啊

int x2d(char x) {
    if (isdigit(x))
        return x - '0';
    else
        return 10 + (x - 'A');
}

int max(int a, int b) { return a >= b ? a : b; }

// 标记 | 组索引 | 块偏移
// Set, E-associativity, Block
// 012 345 678, len = 9
// ttt sss bbb
// TODO: unknown the range of s, e, b, maybe too long to hold
unsigned long long get_addr_s(unsigned long long wrap, int len, int s, int e, int b) {
    unsigned long long mask = ((1LL << s) - 1);
    wrap = (wrap >> b) & mask;
    return wrap;
}
unsigned long long get_addr_t(unsigned long long wrap, int len, int s, int e, int b) {
    wrap = wrap >> (s + b);
    return wrap;
}

typedef struct {
    unsigned long long tag;
    int valid;
    int timestamp;   // hit 和 push 时更新
    uint8_t *blocks; // 实际上不需要
} Line;

Line create_line(size_t b, unsigned long long tag) {
    Line ret;
    ret.tag = tag;
    ret.valid = 0;
    ret.timestamp = -1;
    ret.blocks = (uint8_t *)malloc(sizeof(uint8_t) * b);
    memset(ret.blocks, 0, sizeof(uint8_t) * b);
    return ret;
}

void destory_line(Line *line) {
    if (!line || !line->blocks)
        return;
    free(line->blocks);
    line->blocks = NULL;
}

typedef struct {
    size_t line_count;
    Line *lines;
    // Find line by iterating lines with O(n)
    // 一般是硬件有相应优化, 但毕竟这只是个模拟器
} Set;

Set *create_set(size_t e, size_t b) {
    Set *ret = (Set *)malloc(sizeof(Set));
    if (ret) {
        ret->line_count = 0; // e;
        ret->lines = (Line *)malloc(sizeof(Line) * e);
        if (ret->lines) {
            memset(ret->lines, 0, sizeof(Line) * e);
            for (size_t i = 0; i < e; i++) {
                ret->lines[i] = create_line(b, 0);
            }
            return ret;
        }
    }
    exit(139 + 1);
}

void push_line(Set *set, unsigned long long tag, int e) { // TODO: ?
    // WARN: may eviction
    if (set->line_count == e) {
        evic++;
        // TODO: LRU
        int lru = 0x7fffffff;
        Line *arg = NULL;
        for (int i = 0; i < e; i++) {
            Line *li = &set->lines[i];
            assert(li->valid);
            if (lru > li->timestamp) {
                /*
                 * < : max(|tick - timestamp|)
                 * timestamp 越大越新, 找最旧的也就是最小的
                 * */
                lru = li->timestamp;
                arg = li;
            }
        }
        if (arg) {
            arg->tag = tag;
            arg->timestamp = ++tick;
        }

    } else {
        // TODO: bad impl
        for (int i = 0; i < e; i++) {
            Line *li = &set->lines[i];
            if (!li->valid) {
                li->valid = 1;
                li->tag = tag;
                li->timestamp = ++tick;
                set->line_count++;
                return;
            }
        }
    }
}

void destory_set(Set *set) {
    if (!set || !set->lines)
        return;
    free(set->lines);
    set->lines = NULL;
    free(set);
}

typedef struct {
    size_t set_count;
    Set **sets;
} Cache;

Cache *init_cache(size_t S, size_t e, size_t b) {
    Cache *ret = (Cache *)malloc(sizeof(Cache));
    if (ret) {
        ret->set_count = S;
        ret->sets = (Set **)malloc(sizeof(Set *) * S);
        if (ret->sets) {
            for (size_t i = 0; i < S; i++) {
                ret->sets[i] = create_set(e, b);
            }
            return ret;
        }
    }
    exit(139 + 1);
}

void find_cache(Cache *cache, char *addr, int len, int s, int e, int b) {
    unsigned long long wrap = 0;
    sscanf(addr, "%llx", &wrap);
    unsigned long long set_id = get_addr_s(wrap, len, s, e, b);
    unsigned long long tag = get_addr_t(wrap, len, s, e, b);
    Set *set = cache->sets[set_id];
    for (int i = 0; i < e; i++) {
        Line *li = &set->lines[i];
        if (li->valid && li->tag == tag) {
            hit++;
            li->timestamp = ++tick;
            return;
        }
    }
    // should not be executed if hit
    miss++;
    push_line(set, tag, e);
}

void destory_cache(Cache *cache) {
    if (!cache || !cache->sets)
        return;
    for (size_t i = 0; i < cache->set_count; i++) {
        destory_set(cache->sets[i]);
        cache->sets[i] = NULL;
    }
    free(cache);
}

struct Trace {
    /*
     * Trace type
     * I: Instruction load
     * L: Data load
     * S: Data store
     * M: Modify = load + store
     * */
    char type;
    char addr[MAX_HEX_LEN + 1];
    // assume alignment, ignore `size` field
};

int main(int argc, char **argv) {
    int opt;
    // int verbose = 0;
    int s = 0, E = 0, b = 0;
    FILE *trace_file = NULL;

    while ((opt = getopt(argc, argv, "vs:E:b:t:")) != -1) {
        switch (opt) {
        case 'v':
            // verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = fopen(optarg, "r");
            break;
        default:
            return 1;
        }
    }

    Cache *cache = init_cache((1 << s), E, b);
    // TODO: parse $trace_file
    if (!trace_file)
        exit(139 + 1);
    char trace[256];

    /*
    I  004005c0,5
     S 7ff000398,8
     L 7ff000388,4
     M 7ff000388,4
    */
    char ins;
    char addr[256];
    int blksz;
    while (fgets(trace, sizeof(trace), trace_file)) {
        sscanf(trace, " %c %s,%d", &ins, addr, &blksz);
        int len = strlen(addr);
        switch (ins) {
        case 'I':
            //find_cache(cache, addr, len, s, E, b);
            break;
        case 'S':
            find_cache(cache, addr, len, s, E, b);
            break;
        case 'L':
            find_cache(cache, addr, len, s, E, b);
            break;
        case 'M':
            find_cache(cache, addr, len, s, E, b);
            find_cache(cache, addr, len, s, E, b);
            break;
        }
    }

    printSummary(hit, miss, evic);
    return 0;
}
