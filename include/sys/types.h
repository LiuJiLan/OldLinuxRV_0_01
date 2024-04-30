#ifndef _TYPES_H
#define _TYPES_H

#include <asm/asm.h>

#define NULL ((void *) 0)

typedef char                int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef long long           int64;
typedef unsigned long long  uint64;

typedef int8        s8;
typedef uint8       u8;
typedef int16       s16;
typedef uint16      u16;
typedef int32       s32;
typedef uint32      u32;
typedef int64       s64;
typedef uint64      u64;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

//  平台字长相关(void * 的长度)
typedef long                ptr_t;
typedef unsigned long       uptr_t;

typedef unsigned long       regs_t;
typedef unsigned long       size_t;
typedef long                ssize_t;

#define PGSIZE              PAGE_SIZE
#define PGSHIFT             PAGE_SHIFT


#define PGROUNDUP(sz)       (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a)      (((a)) & ~(PGSIZE-1))


#endif
