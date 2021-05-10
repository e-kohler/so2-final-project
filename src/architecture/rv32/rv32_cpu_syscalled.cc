// EPOS RISC-V 32 System Call Entry Implementation
/*
#include <architecture/rv32/rv32_cpu.h>

extern "C" { void _exec(void *); }

__BEGIN_SYS

// Class attributes
// void * CPU::last_ecall_msg;


void CPU::syscalled() {
    // We get here when an APP triggers INT_SYSCALL (i.e. ecall)
    if(Traits<Build>::MODE == Traits<Build>::KERNEL) {
        _exec(last_ecall_msg); // the message to EPOS Framework is passed on register a1
        CPU::a0(sizeof(void *));                    // tell IC::entry to perform PC = PC + 4 on return
    }
}
__END_SYS
*/