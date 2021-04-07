// EPOS SiFive-E (RISC-V) Memory Map

#ifndef __riscv_memory_map_h
#define __riscv_memory_map_h


#include <system/memory_map.h>


__BEGIN_SYS

struct Memory_Map
{
    enum {
        NOT_USED        = Traits<Machine>::NOT_USED,

        // Physical Memory
        MEM_BASE        = Traits<Machine>::MEM_BASE,
        MEM_TOP         = Traits<Machine>::MEM_TOP,
        MIO_BASE        = Traits<Machine>::MIO_BASE,
        MIO_TOP         = Traits<Machine>::MIO_TOP,
        BOOT_STACK      = Traits<Machine>::BOOT_STACK,

        TEST_BASE       = 0x00100000, // SiFive test engine
        RTC_BASE        = 0x00101000, // goldfish_rtc
        UART_BASE       = 0x10000000, // 16550A NS UART
        CLINT_BASE      = 0x02000000, // Sifive CLINT
        TIMER_BASE      = 0x02004000, // CLINT timer
        PLIIC_CPU_BASE  = 0x0c000000, // SiFive PLIC

        // Logical Address Space
        BOOT            = Traits<Machine>::BOOT,
        IMAGE           = Traits<Machine>::IMAGE,
        SETUP           = Traits<Machine>::SETUP,
        INIT            = Traits<Machine>::INIT,

        APP_LOW         = Traits<Machine>::APP_LOW,
        APP_CODE        = APP_LOW,
        APP_DATA        = APP_LOW,
        APP_HIGH        = Traits<Machine>::APP_HIGH,

        PHY_MEM         = Traits<Machine>::PHY_MEM,
        IO              = Traits<Machine>::IO,
        SYS             = Traits<Machine>::SYS,
        SYS_INFO        = NOT_USED,
        SYS_CODE        = NOT_USED,
        SYS_DATA        = NOT_USED,
        SYS_HEAP        = NOT_USED,
        SYS_STACK       = NOT_USED
    };
};

__END_SYS

#endif
