/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2014 - 2025 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* - Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File      : ARM_Startup.s
Purpose   : Generic startup and exception handlers for ARM devices.

Additional information:
  Preprocessor Definitions
    __FLASH_BUILD
      If defined,
        additional code can be executed in Reset_Handler,
        which might be needed to run from Flash but not to run from RAM.

    __SUPERVISOR_START
      If defined,
        application starts in supervisor mode instead of system mode.

    __HYPERVISOR_START
      If != 0 (default on ARMv8A and ARMv8R)
        Check if system is in Hypervisor mode and leave it.
        Might be necessary for at least ARMv8A/R,
        because stack setup requires mode change, which cannot be
        done in Hypervisor mode.
      If == 0 (default otherwise)
        Do not check mode before stack setup.

    __SOFTFP__
      Defined by the build system.
      If not defined, the FPU is enabled for floating point operations.
*/

        .syntax unified  


#ifndef __HYPERVISOR_START
  #if defined(__ARM_ARCH_8A__) || defined(__ARM_ARCH_8R__)
    #define __HYPERVISOR_START 1
  #else
    #define __HYPERVISOR_START 0
  #endif
#endif
        
/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
//
// Declare an interrupt handler
//
.macro ISR_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        ldr     PC, =\Name
        //
        // Insert dummy handler in init section
        //
        .section .init.\Name, "ax"
        .code 32
        .type \Name, function
        .weak \Name
        .balign 4
\Name:
        1: b 1b   // Endless loop
        END_FUNC \Name
.endm

//
// Place a reserved isr handler
//
.macro ISR_RESERVED
        .section .vectors, "ax"
        nop
.endm

//
// Mark the end of a function and calculate its size
//
.macro END_FUNC name
        .size \name,.-\name
.endm

#if defined(__ARM_ARCH_4T__)
.macro blx reg
  mov lr, pc
  bx \reg
.endm
#endif 

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
/*********************************************************************
*
*  Setup of the exception table
*
*/
        .section .vectors, "ax"
        .code 32
        .balign 4
        .global _vectors
_vectors:
        ldr     PC, =Reset_Handler
        ISR_HANDLER undef_handler
        ISR_HANDLER swi_handler
        ISR_HANDLER pabort_handler
        ISR_HANDLER dabort_handler
        ISR_RESERVED
        #ISR_HANDLER irq_handler
		ldr     PC, =IRQ_Handler
        ISR_HANDLER fiq_handler

        .section .vectors, "ax"
        .size _vectors, .-_vectors
_vectors_end:

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       Reset_Handler
*
*  Function description
*    Exception handler for reset.
*    Generic bringup of a system.
*/
        .section .init.Reset_Handler, "ax"
        .code 32
        .balign 4
        .global reset_handler
        .global Reset_Handler
        .equ reset_handler, Reset_Handler
        .type Reset_Handler, function
Reset_Handler:
	cpsid i						/* 关闭全局中断 */

	/* 关闭I,DCache和MMU 
	 * 采取读-改-写的方式。
	 */
	mrc     p15, 0, r0, c1, c0, 0     /* 读取CP15的C1寄存器到R0中       		        	*/
    bic     r0,  r0, #(0x1 << 12)     /* 清除C1寄存器的bit12位(I位)，关闭I Cache            	*/
    bic     r0,  r0, #(0x1 <<  2)     /* 清除C1寄存器的bit2(C位)，关闭D Cache    				*/
    bic     r0,  r0, #0x2             /* 清除C1寄存器的bit1(A位)，关闭对齐						*/
    bic     r0,  r0, #(0x1 << 11)     /* 清除C1寄存器的bit11(Z位)，关闭分支预测					*/
    bic     r0,  r0, #0x1             /* 清除C1寄存器的bit0(M位)，关闭MMU				       	*/
    mcr     p15, 0, r0, c1, c0, 0     /* 将r0寄存器中的值写入到CP15的C1寄存器中	 				*/

	
#if 0
	/* 汇编版本设置中断向量表偏移 */
	ldr r0, =0X87800000

	dsb
	isb
	mcr p15, 0, r0, c12, c0, 0
	dsb
	isb
#endif
    
	/* 设置各个模式下的栈指针，
	 * 注意：IMX6UL的堆栈是向下增长的！
	 * 堆栈指针地址一定要是4字节地址对齐的！！！
	 * DDR范围:0X80000000~0X9FFFFFFF
	 */
	/* 进入IRQ模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x12 	/* r0或上0x13,表示使用IRQ模式					*/
	msr cpsr, r0		/* 将r0 的数据写入到cpsr_c中 					*/
	ldr sp, =0x80600000	/* 设置IRQ模式下的栈首地址为0X80600000,大小为2MB */

	/* 进入SYS模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x1f 	/* r0或上0x13,表示使用SYS模式					*/
	msr cpsr, r0		/* 将r0 的数据写入到cpsr_c中 					*/
	ldr sp, =0x80400000	/* 设置SYS模式下的栈首地址为0X80400000,大小为2MB */

	/* 进入SVC模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f 	/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x13 	/* r0或上0x13,表示使用SVC模式					*/
	msr cpsr, r0		/* 将r0 的数据写入到cpsr_c中 					*/
	ldr sp, =0X80200000	/* 设置SVC模式下的栈首地址为0X80200000,大小为2MB */

	cpsie i				/* 打开全局中断 */
#if 0
	/* 使能IRQ中断 */
	mrs r0, cpsr		/* 读取cpsr寄存器值到r0中 			*/
	bic r0, r0, #0x80	/* 将r0寄存器中bit7清零，也就是CPSR中的I位清零，表示允许IRQ中断 */
	msr cpsr, r0		/* 将r0重新写入到cpsr中 			*/
#endif

	b _start				/* 跳转到main函数 			 	*/

/* IRQ中断！重点！！！！！ */
IRQ_Handler:
	push {lr}					/* 保存lr地址 */
	push {r0-r3, r12}			/* 保存r0-r3，r12寄存器 */

	mrs r0, spsr				/* 读取spsr寄存器 */
	push {r0}					/* 保存spsr寄存器 */

	mrc p15, 4, r1, c15, c0, 0 /* 从CP15的C0寄存器内的值到R1寄存器中
								* 参考文档ARM Cortex-A(armV7)编程手册V4.0.pdf P49
								* Cortex-A7 Technical ReferenceManua.pdf P68 P138
								*/							
	add r1, r1, #0X2000			/* GIC基地址加0X2000，也就是GIC的CPU接口端基地址 */
	ldr r0, [r1, #0XC]			/* GIC的CPU接口端基地址加0X0C就是GICC_IAR寄存器，
								 * GICC_IAR寄存器保存这当前发生中断的中断号，我们要根据
								 * 这个中断号来绝对调用哪个中断服务函数
								 */
	push {r0, r1}				/* 保存r0,r1 */
	
	cps #0x13					/* 进入SVC模式，允许其他中断再次进去 */
	
	push {lr}					/* 保存SVC模式的lr寄存器 */
	ldr r2, =system_irqhandler	/* 加载C语言中断处理函数到r2寄存器中*/
	blx r2						/* 运行C语言中断处理函数，带有一个参数，保存在R0寄存器中 */

	pop {lr}					/* 执行完C语言中断服务函数，lr出栈 */
	cps #0x12					/* 进入IRQ模式 */
	pop {r0, r1}				
	str r0, [r1, #0X10]			/* 中断执行完成，写EOIR */

	pop {r0}						
	msr spsr_cxsf, r0			/* 恢复spsr */

	pop {r0-r3, r12}			/* r0-r3,r12出栈 */
	pop {lr}					/* lr出栈 */
	subs pc, lr, #4				/* 将lr-4赋给pc */  

/*************************** End of file ****************************/
