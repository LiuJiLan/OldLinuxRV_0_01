#
# Makefile for linux.
# If you don't have '-mstring-insns' in your gcc (and nobody but me has :-)
# remove them from the CFLAGS defines.
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
CFLAGS 	+= -Iinclude
CPP     = ${CROSS_COMPILE}cpp -nostdinc -Iinclude

####额外部分
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump


ARCHIVES=kernel/kernel.o mm/mm.o fs/fs.o
LIBS	=lib/lib.a

%.o : %.c
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o $*.o $<
%.o : %.S	# 不在使用.s, 全部使用能预处理的.S
	@${CC} ${CFLAGS} -c -o $*.o $<

all:	Image

Image: boot/boot tools/system tools/build
	tools/build boot/boot tools/system > Image
	sync

tools/build: tools/build.c
	$(CC) $(CFLAGS) \
	-o tools/build tools/build.c
	chmem +65000 tools/build

boot/head.o: boot/head.S
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o boot/head.o boot/head.S

tools/system:	boot/head.o init/main.o \
		$(ARCHIVES) $(LIBS)
	$(LD) $(LDFLAGS) boot/head.o init/main.o \
	$(ARCHIVES) \
	$(LIBS) \
	-o tools/system > System.map

kernel/kernel.o:
	(cd kernel; make)

mm/mm.o:
	(cd mm; make)

fs/fs.o:
	(cd fs; make)

lib/lib.a:
	(cd lib; make)



clean:
	rm -f system.lds	# new
	rm -f Image System.map tmp_make boot/boot core
	rm -f init/*.o boot/*.o tools/system tools/build
	(cd mm;make clean)
	(cd fs;make clean)
	(cd kernel;make clean)
	(cd lib;make clean)


dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in init/*.c;do echo -n "init/";$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile
	#(cd fs; make dep)
	(cd kernel; make dep)
	(cd mm; make dep)

# 可以这样来在链接脚本中引入头文件
# -P不会生成#开头的额外注释
system.lds:
	${CPP} -P ./kernel/system.lds.S -o system.lds

DEBUG = ./debug

QEMU = qemu-system-riscv64
QFLAGS = -smp 2 -M virt -bios default
QFLAGS += -m 128M -nographic
QFLAGS += -monitor telnet:127.0.0.1:5555,server,nowait
# telnet 127.0.0.1 5555

tools/system.elf: system.lds boot/head.o init/main.o \
	$(ARCHIVES)  $(LIBS)
	$(LD) $(LDFLAGS) -T system.lds \
	boot/head.o init/main.o	\
	$(ARCHIVES) \
	$(LIBS) \
    -o tools/system.elf > System.map

tools/kernel.elf: tools/system.elf
	#@${CC} ${CFLAGS} -T kernel.ld -o kernel.elf $^
	cp tools/system.elf tools/kernel.elf
	${OBJCOPY} -O binary tools/kernel.elf tools/kernel.bin

GDB = ${CROSS_COMPILE}gdb
READELF = ${CROSS_COMPILE}readelf

QFLAGS += -kernel tools/kernel.elf
QFLAGS += -drive file=./debug/hdc2.img,if=none,format=raw,id=x0
QFLAGS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0


.DEFAULT_GOAL := tools/kernel.elf

debug: tools/kernel.elf
	$(OBJDUMP) -D -b binary -m riscv tools/kernel.bin > $(DEBUG)/dis.asm
	$(OBJDUMP) -S tools/kernel.elf > $(DEBUG)/kernel.asm
	$(READELF) -a -W tools/kernel.elf > $(DEBUG)/kernel.txt
	${QEMU} ${QFLAGS} -s -S &
	${GDB} tools/kernel.elf -q -x $(DEBUG)/gdbinit.txt

### Dependencies:
init/main.o: init/main.c include/linux/tty.h include/termios.h \
 include/linux/sched.h include/asm/ptrace.h include/sys/types.h \
 include/asm/asm.h include/asm/asm_offsets.h include/asm/system.h \
 include/linux/config.h include/linux/fs.h include/linux/mm.h \
 include/linux/kernel.h include/stdarg.h include/asm/sbi.h
