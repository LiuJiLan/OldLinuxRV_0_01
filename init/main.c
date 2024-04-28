unsigned long boot_cpu_hartid;

static inline void ebreak() {
    asm volatile("ebreak");
}

void start_kernel(void){
    // 检查sp应该是相同的
    ebreak();
    ebreak();

    while (1) {

    }

}