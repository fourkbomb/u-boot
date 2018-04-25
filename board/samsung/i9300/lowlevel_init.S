#include <config.h>

#define CLOCK_BASE 0x10030000
#define CLOCK_SRC_PERIL0_OFFSET 0xc250
#define CLOCK_SRC_PERIL0_VAL 0x666666 @ sclkmpll_user_t
#define CLOCK_DIV_PERIL0_OFFSET 0xc550
#define CLOCK_DIV_PERIL0_VAL 0x888888

#define UART2_BASE 0x13820000
#define ULCON 0x0
#define UCON 0x4
#define UFCON 0x8
#define UFSTAT 0x18
#define UTXH 0x20
#define URXH 0x24
#define UBRDIV 0x28
#define UFRACVAL 0x2c

#define UBRDIV_VAL 0x2c
#define UDIVSLOT_VAL 0xc

.globl lowlevel_init
lowlevel_init:

    ldr sp, =0x02060000 @ use IROM stack in BL2
    push {lr}

    ldr r0, =0x10500054 @ Cortex-A9 SCU
    ldr r1, [r0]
    orr r1, r1, #0xf @ allow non-secure access to CPU[0-3] components
    str r1, [r0]

    mov r1, #0 @ power down
    ldr r0, =0x10021174 @ RESET_ISP_SYS_PWR_REG
    str r1, [r0]
    ldr r0, =0x100213b8 @ SYSCLK_ISP_SYS_PWR_REG
    str r1, [r0]

    ldr r0, =0x10020804 @ INFORM1
    ldr r1, [r0] @ reset status
/*
    ldr r2, =S5P_CHECK_SLEEP
    cmp r1, r2
    beq wakeup_reset
*/
    ldr r0, =0x1002330c
    ldr r1, [r0]
    orr r1, r1, #0x300
    str r1, [r0] @ hold PS_HOLD high

    bl uart_asm_init

#ifndef CONFIG_SPL_BUILD
    b not_spl
#endif

    bl mem_asm_init

    bl system_clock_init_asm
/*
    ldr r0, =0x40000000
    cmp r0, pc
    bgt not_spl
*/
uart_bl2_put:
    mov r0, #'B'
    bl uart_putc
    mov r0, #'L'
    bl uart_putc
    mov r0, #'2'
    bl uart_putc
    mov r0, #' '
    bl uart_putc
    mov r0, #'!'
    bl uart_putc
    mov r0, #' '
    bl uart_putc
    mov r0, #'H'
    bl uart_putc
    mov r0, #'i'
    bl uart_putc

    ldr r0, =0x10000000
9:    subs r0, r0, #1
    bne 9b

    pop {pc}

not_spl:
    mov r0, #'X'
    bl uart_putc
    mov r0, #'L'
    bl uart_putc
    mov r0, #'3'
    bl uart_putc
    mov r0, #' '
    bl uart_putc
    mov r0, #'!'
    bl uart_putc
    mov r0, #' '
    bl uart_putc
    mov r0, #'B'
    bl uart_putc
    mov r0, #'3'
    bl uart_putc

    pop {pc}



.globl uart_putc
uart_putc: @ ARM calling convention: must preserve r4-r11
    mov r2, r0
    ldr r0, =UART2_BASE
1:
    ldr r1, [r0, #UFSTAT]
    lsr r1, r1, #24
    and r1, r1, #1
    cmp r1, #1 @ Tx FIFO Full
    beq 1b

    str r2, [r0, #UTXH]
    mov pc, lr

.globl uart_asm_init
uart_asm_init:
    /* GPIO init */
    ldr r0, =0x11400000
    ldr r1, =0x22222222
    str r1, [r0]
    ldr r0, =0x11400020
    ldr r1, =0x00222222
    str r1, [r0]

    ldr r0, =CLOCK_BASE
    ldr r1, =CLOCK_SRC_PERIL0_OFFSET
    ldr r2, =CLOCK_SRC_PERIL0_VAL
    str r2, [r0, r1]

    ldr r1, =CLOCK_DIV_PERIL0_OFFSET
    ldr r2, =CLOCK_DIV_PERIL0_VAL
    str r2, [r0, r1]

    ldr r0, =UART2_BASE
    ldr r1, =0x111
    str r1, [r0, #UFCON]

    mov r1, #0x3
    str r1, [r0, #ULCON]

    ldr r1, =0x3c5
    str r1, [r0, #UCON]

    mov r1, #UBRDIV_VAL 
    str r1, [r0, #UBRDIV]

    mov r1, #UDIVSLOT_VAL
    str r1, [r0, #UFRACVAL]

    /* Send "O" */
    ldr r1, =0x4f4f4f4f
    str r1, [r0, #UTXH]

    mov pc, lr

