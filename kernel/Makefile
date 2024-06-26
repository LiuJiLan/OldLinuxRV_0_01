#
# Makefile for the FREAX-kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#

# 编译工具部分
CROSS_COMPILE = riscv64-unknown-elf-

AS		= ${CROSS_COMPILE}as
LD 		= ${CROSS_COMPILE}ld

# LDFLAGS	=-s -x -M
LDFLAGS = -M

CC 		= ${CROSS_COMPILE}gcc
CFLAGS 	= -Wall -g -ggdb
CFLAGS 	+= -nostdlib -fno-builtin -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS 	+= -mcmodel=medany -mabi=lp64f -march=rv64imafc
CFLAGS 	+= -I../include
CPP     = ${CROSS_COMPILE}cpp -nostdinc -I../include

%.o : %.c
	$(CC) $(CFLAGS)  -c -o $*.o $<
%.o : %.S	# 不在使用.s, 全部使用能预处理的.S
	@${CC} ${CFLAGS} -c -o $*.o $<

#OBJS  = sched.o system_call.o traps.o asm.o fork.o \
#	panic.o printk.o vsprintf.o tty_io.o console.o \
#	keyboard.o rs_io.o hd.o sys.o exit.o serial.o \
#	mktime.o

OBJS  = sched.o system_call.o traps.o entry.o irq.o fork.o \
		panic.o printk.o vsprintf.o tty_io.o signal.o \
		rs_io.o hd.o virtio.o sys.o exit.o serial.o \
		sbi.o

kernel.o: $(OBJS)
	$(LD) -r -o kernel.o $(OBJS)
	sync

clean:
	rm -f core *.o *.a tmp_make
	for i in *.c;do rm -f `basename $$i .c`.s;done

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
exit.s exit.o: exit.c ../include/errno.h ../include/signal.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/ptrace.h ../include/asm/system.h ../include/sys/wait.h \
 ../include/linux/sched.h ../include/linux/config.h ../include/linux/fs.h \
 ../include/linux/mm.h ../include/linux/kernel.h ../include/stdarg.h \
 ../include/string.h
fork.s fork.o: fork.c ../include/errno.h ../include/linux/sched.h \
 ../include/asm/ptrace.h ../include/sys/types.h ../include/asm/asm.h \
 ../include/asm/asm_offsets.h ../include/asm/system.h \
 ../include/linux/config.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/kernel.h ../include/stdarg.h ../include/string.h
hd.s hd.o: hd.c ../include/linux/config.h ../include/linux/sched.h \
 ../include/asm/ptrace.h ../include/sys/types.h ../include/asm/asm.h \
 ../include/asm/asm_offsets.h ../include/asm/system.h \
 ../include/linux/fs.h ../include/linux/mm.h ../include/linux/kernel.h \
 ../include/stdarg.h ../include/linux/hdreg.h ../include/linux/trap.h \
 ../include/string.h ../include/linux/virtio.h
irq.s irq.o: irq.c ../include/linux/trap.h ../include/asm/ptrace.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/system.h ../include/linux/config.h \
 ../include/linux/sched.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/sys.h ../include/linux/kernel.h ../include/stdarg.h
panic.s panic.o: panic.c ../include/linux/kernel.h ../include/stdarg.h
printk.s printk.o: printk.c ../include/stdarg.h ../include/linux/kernel.h
rs_io.s rs_io.o: rs_io.c ../include/linux/config.h ../include/linux/sched.h \
 ../include/asm/ptrace.h ../include/sys/types.h ../include/asm/asm.h \
 ../include/asm/asm_offsets.h ../include/asm/system.h \
 ../include/linux/fs.h ../include/linux/mm.h ../include/linux/tty.h \
 ../include/termios.h ../include/linux/kernel.h ../include/stdarg.h \
 ../include/linux/hdreg.h ../include/linux/trap.h ../include/string.h
sbi.s sbi.o: sbi.c ../include/asm/sbi.h ../include/sys/types.h \
 ../include/asm/asm.h ../include/asm/asm_offsets.h
sched.s sched.o: sched.c ../include/linux/sched.h ../include/asm/ptrace.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/system.h ../include/linux/config.h ../include/linux/fs.h \
 ../include/linux/mm.h ../include/linux/trap.h ../include/signal.h
serial.s serial.o: serial.c ../include/linux/tty.h ../include/termios.h \
 ../include/linux/sched.h ../include/asm/ptrace.h ../include/sys/types.h \
 ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/system.h ../include/linux/config.h ../include/linux/fs.h \
 ../include/linux/mm.h ../include/linux/trap.h
signal.s signal.o: signal.c ../include/signal.h ../include/sys/types.h \
 ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/ptrace.h ../include/asm/system.h ../include/linux/sched.h \
 ../include/linux/config.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/kernel.h ../include/stdarg.h
sys.s sys.o: sys.c ../include/errno.h ../include/linux/sched.h \
 ../include/asm/ptrace.h ../include/sys/types.h ../include/asm/asm.h \
 ../include/asm/asm_offsets.h ../include/asm/system.h \
 ../include/linux/config.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/tty.h ../include/termios.h ../include/linux/kernel.h \
 ../include/stdarg.h ../include/sys/times.h ../include/sys/utsname.h
system_call.s system_call.o: system_call.c ../include/asm/ptrace.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/system.h ../include/linux/trap.h \
 ../include/linux/config.h ../include/linux/sched.h ../include/linux/fs.h \
 ../include/linux/mm.h ../include/linux/sys.h ../include/signal.h
traps.s traps.o: traps.c ../include/linux/trap.h ../include/asm/ptrace.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/asm/system.h ../include/linux/config.h \
 ../include/linux/sched.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/kernel.h ../include/stdarg.h
tty_io.s tty_io.o: tty_io.c ../include/ctype.h ../include/errno.h \
 ../include/signal.h ../include/sys/types.h ../include/asm/asm.h \
 ../include/asm/asm_offsets.h ../include/asm/ptrace.h \
 ../include/asm/system.h ../include/linux/sched.h \
 ../include/linux/config.h ../include/linux/fs.h ../include/linux/mm.h \
 ../include/linux/tty.h ../include/termios.h ../include/linux/trap.h
virtio.s virtio.o: virtio.c ../include/linux/virtio.h ../include/sys/types.h \
 ../include/asm/asm.h ../include/asm/asm_offsets.h \
 ../include/linux/config.h ../include/linux/kernel.h ../include/stdarg.h
vsprintf.s vsprintf.o: vsprintf.c ../include/stdarg.h ../include/string.h \
 ../include/sys/types.h ../include/asm/asm.h ../include/asm/asm_offsets.h
