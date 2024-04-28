#ifndef _ASM_OFFSETS_H
#define _ASM_OFFSETS_H

// for pt_regs

#define PT_SIZE 288

#define PT_EPC 0
#define PT_RA 8
#define PT_FP 64
#define PT_S0 64
#define PT_S1 72
#define PT_S2 144
#define PT_S3 152
#define PT_S4 160
#define PT_S5 168
#define PT_S6 176
#define PT_S7 184
#define PT_S8 192
#define PT_S9 200
#define PT_S10 208
#define PT_S11 216
#define PT_SP 16
#define PT_TP 32
#define PT_A0 80
#define PT_A1 88
#define PT_A2 96
#define PT_A3 104
#define PT_A4 112
#define PT_A5 120
#define PT_A6 128
#define PT_A7 136
#define PT_T0 40
#define PT_T1 48
#define PT_T2 56
#define PT_T3 224
#define PT_T4 232
#define PT_T5 240
#define PT_T6 248
#define PT_GP 24
#define PT_ORIG_A0 280
#define PT_STATUS 256
#define PT_BADADDR 264
#define PT_CAUSE 272

// thread_struct

#define THREAD_RA 0
#define THREAD_SP 8
#define THREAD_S0 16
#define THREAD_S1 24
#define THREAD_S2 32
#define THREAD_S3 40
#define THREAD_S4 48
#define THREAD_S5 56
#define THREAD_S6 64
#define THREAD_S7 72
#define THREAD_S8 80
#define THREAD_S9 88
#define THREAD_S10 96
#define THREAD_S11 104
#define THREAD_F0 112
#define THREAD_F1 120
#define THREAD_F2 128
#define THREAD_F3 136
#define THREAD_F4 144
#define THREAD_F5 152
#define THREAD_F6 160
#define THREAD_F7 168
#define THREAD_F8 176
#define THREAD_F9 184
#define THREAD_F10 192
#define THREAD_F11 200
#define THREAD_F12 208
#define THREAD_F13 216
#define THREAD_F14 224
#define THREAD_F15 232
#define THREAD_F16 240
#define THREAD_F17 248
#define THREAD_F18 256
#define THREAD_F19 264
#define THREAD_F20 272
#define THREAD_F21 280
#define THREAD_F22 288
#define THREAD_F23 296
#define THREAD_F24 304
#define THREAD_F25 312
#define THREAD_F26 320
#define THREAD_F27 328
#define THREAD_F28 336
#define THREAD_F29 344
#define THREAD_F30 352
#define THREAD_F31 360
#define THREAD_FCSR 368

// thread_info

#define TASK_TI_FLAGS 0
#define TASK_TI_KERNEL_SP 8
#define TASK_TI_USER_SP 16
#define TASK_TI_CPU 24

#endif
