#
# Makefile for some libs needed in the kernel.
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

OBJS  = ctype.o _exit.o open.o close.o errno.o write.o dup.o setsid.o \
	execve.o wait.o string.o

lib.a: $(OBJS)
	$(AR) rcs lib.a $(OBJS)
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
