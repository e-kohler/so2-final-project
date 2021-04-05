// EPOS RISC-V sifive SETUP

#include <system/config.h>
#include <architecture/cpu.h>
#include <architecture/mmu.h>
#include <machine/timer.h>
#include <machine/ic.h>

using namespace EPOS::S;
typedef unsigned int Reg;

extern "C" 
{
    void _setup() __attribute__ ((used, naked, section(".init")));
    void _mmode_forward() {
        if (CPU::int_enabled())
            CPU::sip(CPU::STI);
        ASM("mret");
    }
    void _int_m2s();
    void _int_entry();
    void _start();
    void _wait() 
    { 
        CPU::halt();
        _start();
    }
}

__BEGIN_SYS

class Setup_SifiveE {
private:
    typedef CPU::Reg Reg;
    typedef MMU::RV32_Flags RV32_Flags;
    typedef MMU::Page_Table Page_Table;
    typedef MMU::Page_Directory Page_Directory;
    typedef MMU::PT_Entry PT_Entry;

public:
    static void init() { setup_machine_environment(); }
    static void setup_machine_environment();
    static void setup_supervisor_environment();
    static void build_page_tables();
};

void Setup_SifiveE::build_page_tables() 
{
    Reg page_tables = Traits<Machine>::PAGE_TABLES; // address of the page table root
    MMU::_master = new ( (void *) page_tables ) Page_Directory();

    for(int i = 0; i < 1024; i++) { 
        PT_Entry * pte = (((PT_Entry *)MMU::_master) + i);
        * pte = ((page_tables >> 12) << 10);
        * pte += ((i+1) << 10);
        * pte |= MMU::RV32_Flags::VALID;    
    }

    for(int i = 0; i < 1024; i++)
    {
        Page_Table * pt = new ( (void *)(page_tables + 4*1024*(i+1))  ) Page_Table();
        pt->setflags(RV32_Flags::SYS);
    }
}

void Setup_SifiveE::setup_supervisor_environment() 
{
    CPU::satp_write((0x1 << 31) | (Traits<Machine>::PAGE_TABLES >> 12));
    ASM("sret");
}

void Setup_SifiveE::setup_machine_environment()
{
    CPU::mmode_int_disable();
    Reg core = CPU::mhartid();
    CPU::tp(core);
    // set stack for each core
    CPU::sp(Traits<Machine>::BOOT_STACK - Traits<Machine>::STACK_SIZE * core);
    CPU::mstatus_write(CPU::MPP_S | CPU::MPIE);
    CPU::mepc((unsigned)&setup_supervisor_environment);
    CPU::mtvec((unsigned)&_int_m2s & 0xfffffffc);
    CPU::mie_write(CPU::MTI);

    build_page_tables();
    CPU::satp_write(0); // paging off
    
    // forward everything
    CPU::sstatus_write( CPU::SIE | CPU::SPIE | CPU::SPP_S );
    CPU::sie_write( CPU::SSI | CPU::STI | CPU::SEI );
    CPU::stvec_write((unsigned)&_int_entry & 0xfffffffc);
    CPU::sepc_write((unsigned)&_start);
    
    // delegate everything
    CPU::mideleg_write(CPU::SSI | CPU::STI | CPU::SEI);
    CPU::medeleg_write(0xffff);

    ASM("mret");
}

__END_SYS

void _setup() { 
    ASM("la      sp, __boot_stack__");
    Setup_SifiveE::init(); 
}
