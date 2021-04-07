// EPOS SiFive-U (RISC-V) Memory Map

#ifndef __riscv_memory_map_h
#define __riscv_memory_map_h


#include <system/memory_map.h>

__BEGIN_SYS

struct Memory_Map
{
    enum {
        NOT_USED        = Traits<Machine>::NOT_USED,

        // Physical Memory
        MIO_BASE        = Traits<Machine>::MIO_BASE,
        MIO_TOP         = Traits<Machine>::MIO_TOP,
        MEM_BASE        = Traits<Machine>::MEM_BASE,
        MEM_TOP         = Traits<Machine>::MEM_TOP,
        BOOT_STACK      = Traits<Machine>::BOOT_STACK,

        // Memory-mapped devices
        TEST_BASE       = 0x00100000, // SiFive test engine
        RTC_BASE        = 0x00101000, // Goldfish RTC
        UART_BASE       = 0x10000000, // NS16550A UART
        CLINT_BASE      = 0x02000000, // SiFive CLINT
        TIMER_BASE      = 0x02004000, // CLINT Timer
        PLIIC_CPU_BASE  = 0x0c000000, // SiFive PLIC

        // Logical Address Space
        BOOT            = Traits<Machine>::BOOT,
        IMAGE           = Traits<Machine>::IMAGE,
        SETUP           = Traits<Machine>::SETUP,
        INIT            = Traits<Machine>::INIT,

        APP_LOW         = Traits<Machine>::APP_LOW,
        APP_CODE        = APP_LOW,
        APP_DATA        = APP_LOW + 4 * 1024 * 1024,
        APP_HIGH        = Traits<Machine>::APP_HIGH,

        PHY_MEM         = Traits<Machine>::PHY_MEM,
        IO              = Traits<Machine>::IO,

        SYS             = Traits<Machine>::SYS,
        SYS_CODE        = SYS + 0x00000000,
        SYS_INFO        = SYS + 0x00100000,
        SYS_PT          = SYS + 0x00101000,
        SYS_PD          = SYS + 0x00102000,
        SYS_DATA        = SYS + 0x00103000,
        SYS_STACK       = SYS + 0x00200000,
        SYS_HEAP        = NOT_USED
    };
};

__END_SYS

#endif
