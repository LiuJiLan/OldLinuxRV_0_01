#define V_P_DIFF    0xFFFFFFC000000000U

// 还是沿用xv6-riscv的宏
#define V2P(a) (((unsigned long) (a)) - V_P_DIFF)   //  64位下改了数据类型
#define P2V(a) ((void *)(((unsigned char *) (a)) + V_P_DIFF))

//  纯数值的转换
#define V2P_WO(x) ((x) - V_P_DIFF)    // same as V2P, but without casts //  不带类型转换
#define P2V_WO(x) ((x) + V_P_DIFF)    // same as P2V, but without casts

.text

.global _start
_start = V2P_WO(entry)

.global for_gdb_physical
for_gdb_physical = V2P_WO(for_gdb_virtual)

.global entry

entry:
    // You Spin Me Right Round
    j entry


for_gdb_virtual:
    //  用于调试
