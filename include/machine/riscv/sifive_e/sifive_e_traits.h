// EPOS SiFive-E (RISC-V) Metainfo and Configuration

#ifndef __riscv_sifive_e_traits_h
#define __riscv_sifive_e_traits_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common;
template<> struct Traits<Machine_Common>: public Traits<Build> {};

template<> struct Traits<Machine>: public Traits<Machine_Common>
{
    static const bool cpus_use_local_timer      = false;

    static const unsigned int NOT_USED          = 0xffffffff;
    static const unsigned int CPUS              = Traits<Build>::CPUS;

    // Physical Memory
    static const unsigned int MEM_BASE          = 0x80000000;   // 2 GB
    static const unsigned int MEM_TOP           = 0x87ffffff;   // 2 GB + 128 MB (128 MB of RAM)
    static const unsigned int MIO_BASE          = 0x00000000;
    static const unsigned int MIO_TOP           = 0x100081ff;

    // Boot Image
    static const unsigned int BOOT_LENGTH_MIN   = NOT_USED;
    static const unsigned int BOOT_LENGTH_MAX   = NOT_USED;
    static const unsigned int BOOT_STACK        = MEM_TOP;      // This will be used as the stack pointer, not the base

    // Logical Memory Map
    static const unsigned int BOOT              = NOT_USED;     // Not needed for this machine
    static const unsigned int IMAGE             = NOT_USED;     // The address for the boot image is defined, but not used in this machine
    static const unsigned int SETUP             = NOT_USED;     // Only Library mode is supported
    static const unsigned int INIT              = NOT_USED;     // Only Library mode is supported

    static const unsigned int APP_LOW           = MEM_BASE;
    static const unsigned int APP_HIGH          = MEM_TOP;

    static const unsigned int PHY_MEM           = NOT_USED;     // Only Library mode is supported
    static const unsigned int IO                = NOT_USED;     // Only Library mode is supported
    static const unsigned int SYS               = NOT_USED;     // Only Library mode is supported

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE        = 16 * 1024;
    static const unsigned int HEAP_SIZE         = 16 * 1024 * 1024;
    static const unsigned int MAX_THREADS       = 16;
};

template <> struct Traits<IC>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    static const unsigned int IRQS = 1024; // PLIC
    static const unsigned int INTS = 1056; // Exceptions + Software + Local + Timer + External
};

template <> struct Traits<Timer>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    static const unsigned int UNITS = 1;
    static const unsigned int CLOCK = 10000000;

    // Meaningful values for the timer frequency range from 100 to 10000 Hz. The
    // choice must respect the scheduler time-slice, i. e., it must be higher
    // than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<UART>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 2;

    static const unsigned int REFERENCE_CLOCK = 22729000;
    static const unsigned int CLOCK_DIVISOR = 16;
    static const unsigned int CLOCK = REFERENCE_CLOCK/CLOCK_DIVISOR;

    static const unsigned int DEF_UNIT = 1;
    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<Serial_Display>: public Traits<Machine_Common>
{
    static const bool enabled = (Traits<Build>::EXPECTED_SIMULATION_TIME != 0);
    static const int ENGINE = UART;
    static const int UNIT = 1;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

template<> struct Traits<Scratchpad>: public Traits<Machine_Common>
{
    static const bool enabled = true;
};

__END_SYS

#endif
