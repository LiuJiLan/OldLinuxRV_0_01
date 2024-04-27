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
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o $*.o $<
%.o : %.S	# 不在使用.s, 全部使用能预处理的.S
	@${CC} ${CFLAGS} -c -o $*.o $<

#OBJS  = sched.o system_call.o traps.o asm.o fork.o \
#	panic.o printk.o vsprintf.o tty_io.o console.o \
#	keyboard.o rs_io.o hd.o sys.o exit.o serial.o \
#	mktime.o

OBJS  = sched.o

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
