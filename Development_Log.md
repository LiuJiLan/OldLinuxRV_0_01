# Log 0 项目的准备

以Linux 0.01的原文件结构建好文件夹, build文件夹不再使用。根据上一个项目的经验保留了tools并加入了debug文件夹。

为每一子文件夹创建对应的Makefile, 修改原Linux 0.01中过时的编译FLAG, 并加入RISC-V特有的FLAG。

引入`.lds.S`类型文件, 方便在编译脚本中使用宏, 加强编译脚本的灵活性。



# Log 1 内核的汇编级启动

内核的汇编级启动涉及到文件head.S。

head.S流程中暂时简化了RISC-V Linux的启动流程。不考虑XIP; 仅支持`Ordered booting`; 仅支持单核, 仅支持sv39。且仅接受以非内置设备树的形式接受外设信息(虽然大概率不会去解析就是了)。

主要流程为: 启动, 关一系列中断, 清除BSS, 转跳到正确的运行地址, 进入C语言主程序。

涉及以下子问题的设计:

1. `trampoline_pg_dir` 如何设计? 
2. 要提前进一次C语言设置好页表吗?
3. pt_regs结构体如何设计? 和Linux一样就好
4. 进程描述符的大小如何? 8KiB



## 1 临时页表使用:

像xv6-k210一样, 临时页表仅使用一张sv39页表并使用大页能映射1GiB的空间, 对内核来说绰绰有余。仅需要设置少数表项也意味着可以用汇编来应付不同内存起点的问题。且这份页表会常驻内存作为未来多核启动的转跳用页表, 仅用一页不会心疼。唯一的问题可能就是由SBI占用了恼人的2MiB (rv32则是4MiB)。

但考虑到后期的可扩展性, 如果采取sv48或sv57的分页模式, 仅使用一张页表会导致颗粒度过大。毕竟sv48单个页表项会导致512GiB对齐, 而sv57会导致256TiB的对齐。到时候总是要回到1GiB对齐的这个区间上来的。在这种情况下再多使用一张页表将颗粒度降到PMD的级别似乎也并无区别。且`trampoline_pg_dir` 作为静态分配的页表的数量也将会是恒定的, 以PMD作为页表意味着在rv64下至少能保证1GiB的空间被覆盖。这对内核来说足够了。



## 2 setup_vm()的C函数必要性

如果采取曾经考虑过的单张页表的方案, 没有必要C函数。

打算以PMD为颗粒度, 便意味着要设置更多的页表项。此时使用C语言来处理是一个好选择, 同时也保证了不同内存起点与大小的灵活性。未来也可以直接在这个函数中添加dtb的解析。但不是在Linux 0.01的移植中, 因为Linux 0.01自身是手动设置的内存, 这个地方可以复古点。



# Log 2 进入路径和退出路径 + 异常/中断处理

进入路径和退出路径可以直接copy RISC-V Linux。



page.s, system_call.s, rs_io.s, keyboard.s, asm.s 都被归入entry.S中。



Linux RISC-V为每个异常都设置了对应的处理, 这是没有必要的
$$
\text{Exceptions}\begin{cases}
  
  \text{Processor detected}\begin{cases}
  
  	\text{Illegal instruction}\\
  
    \text{address misaligned}\begin{cases}
    \text{Instruction address misaligned : 0}\\
    \text{Load address misaligned				 : 4}\\
    \text{Store/AMO address misaligned	 : 6}\\
    \end{cases}\\
    
    \\
    
    \text{access fault}\begin{cases}
    \text{Instruction access fault 	: 1}\\
    \text{Load access fault 				: 5}\\
    \text{Store/AMO access fault 		: 7}\\
    \end{cases}\\
    
    \\
    \text{page fault}\begin{cases}
    \text{Instruction page fault 	: 12}\\
    \text{Load page fault 				: 13}\\
    \text{Store/AMO page fault 		: 15}\\
    \end{cases}\\
  
  \end{cases}\\

  \\

  \text{Programmed}\begin{cases}
  
    \text{ecall}\begin{cases}
    \text{Environment call from U-mode : 8}\\
    \text{Environment call from S-mode : 9}\\
    \text{Environment call from M-mode : 11}\\
    \end{cases}\\
    
    \\
    
    \text{ebreak}\begin{cases}
    \text{Breakpoint : 3}\\
    \end{cases}\\
  
  \end{cases}\\
  
  
\end{cases}\\
$$
