#include <linux/config.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <linux/hdreg.h>
#include <asm/system.h>
#include <linux/trap.h>

#include <string.h>

#define UART0_BASE_ADDR 0x10000000UL
#define REG_WIDTH 1
#define REG_SHIFT 0
typedef uint8_t* UART_REG;

#define size	 1024
#define startup	 256

// VF2
//#define REG_WIDTH 4
//#define REG_SHIFT 2
//typedef uint32_t* UART_REG;

static inline uint8_t uart_read_reg(uint8_t reg)
{
volatile UART_REG addr = (UART_REG)(D_P2V_WO(UART0_BASE_ADDR) + (reg << REG_SHIFT));
return *addr & 0xff;
}

static inline void uart_write_reg(uint8_t reg, uint8_t value)
{
    volatile UART_REG addr = (UART_REG)(D_P2V_WO(UART0_BASE_ADDR) + (reg << REG_SHIFT));
    *addr = value;
}

#define RBR 0x00
#define THR 0x00
#define IER 0x01
#define IIR 0x02
#define LCR 0x03
#define MCR 0x04
#define LSR 0x05
#define MSR 0x06
#define SCR 0x07
#define DLL 0x00
#define DLM 0x01

#define LSR_RX_READY 0x01
#define LSR_TX_IDLE 0x20

#define IIR_INTR 0x01 // 0位=0有中断, =1无中断
#define IIR_TYPE_MASK 0x06 // 1位和2位来判断类型

void rs_write(struct tty_struct * tty)
{
    cli();
    if (!EMPTY(tty->write_q)) {
        // outb(inb_p(tty->write_q.data+1)|0x02,tty->write_q.data+1);
        uint8_t IER_reg = uart_read_reg(IER);
        uart_write_reg(IER, IER_reg | 0x02);
    }
    sti();
}

void rs_io_init(void) {
    // 省去了对波特率的初始化

    // 如果是对VF2移植, 这两行就注释掉
    // 直接用初始化好的就行了
    uint8_t lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));

    uint8_t IER_reg = uart_read_reg(IER);
    uart_write_reg(IER, IER_reg | 0x0d); // 开write之外所有的中断
}

void read_char(void) {
    uint8_t RBR_reg = uart_read_reg(RBR);
    struct tty_queue * tty_q = table_list[3];
    tty_q->buf[tty_q->head] = RBR_reg;
    if (((tty_q->head + 1) & (size - 1)) != tty_q->tail) {
        tty_q->head = (tty_q->head + 1) & (size - 1);
    }
    do_tty_interrupt(63); // rs1的编号
}

void write_char(void) {
    struct tty_queue * tty_q = table_list[4];
    unsigned long nr = (tty_q->head - tty_q->tail) & (size - 1);
    uint8_t IER_reg;

    if (nr == 0) {
        if (tty_q->proc_list != NULL) {
            tty_q->proc_list->state = TASK_RUNNING;
        }

        IER_reg = uart_read_reg(IER);
        uart_write_reg(IER, IER_reg | 0x0d); // 重新关闭write中断

        return;
    }

    if (nr <= startup) {
        if (tty_q->proc_list != NULL) {
            tty_q->proc_list->state = TASK_RUNNING;
        }
    }

    uart_write_reg(THR, tty_q->buf[tty_q->tail]);

    tty_q->tail = (tty_q->tail - 1) & (size - 1);

    if (tty_q->head == tty_q->tail) {
        if (tty_q->proc_list != NULL) {
            tty_q->proc_list->state = TASK_RUNNING;
        }

        IER_reg = uart_read_reg(IER);
        uart_write_reg(IER, IER_reg | 0x0d); // 重新关闭write中断
    }
}

void rs_interrupt(struct pt_regs * regs) {
    uint8_t IIR_reg;
    while (1) {
        IIR_reg = uart_read_reg(IIR);
        if (!(IIR_reg & IIR_INTR)) {
            break;
        }
        if (IIR_reg > 6) {
            /* this shouldn't happen, but ... */
            break;
        }

        switch (IIR_reg && IIR_TYPE_MASK) {
            case 0x00:
                // = 00 MODEM 状态改变中断，优先级 4。读 MODEM 状态可复

                // 原 modem_status
                /* clear intr by reading modem status reg */
                uart_read_reg(MSR);
                break;

            case 0x02: // 写
                // = 01 发送保持寄存器空中断，优先级 3。写发送保持可复位；
                write_char();
                break;

            case 0x04: // 读
                // = 10 已接收到数据中断，优先级 2。读接收数据可复位；
                read_char();
                break;

            case 0x06:
                // = 11 接收状态有错中断，优先级最高。读线路状态可复位；

                // 原 line_status
                /* clear intr by reading line status reg. */
                uart_read_reg(LSR);
                break;

            default:
                break;
        }

    }
}


#undef size	 1024
#undef startup	 256