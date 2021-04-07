// EPOS SiFive-E (RISC-V) SETUP

#include <architecture.h>
#include <machine.h>

extern "C" void _start();
extern "C" void _int_entry();

extern "C" void _setup() __attribute__ ((used, naked, section(".init")));

void setup();
void wait();

__BEGIN_SYS

class Setup
{
public:
    Setup(char * bi);
};

Setup::Setup(char * bi)
{
    CPU::int_disable(); // will be reenabled at Init_First

    Display::init();
    db<Setup>(TRC) << "SETUP()" << endl;

    IC::init();
    IC::int_vector(IC::INT_RESCHEDULER, IC::ipi_eoi);
    IC::enable(IC::INT_RESCHEDULER);

    _start();
}

__END_SYS

using namespace EPOS;

void _setup()
{
    CPU::mstatusc(CPU::MIE);                    // disable interrupts
    CPU::mies(CPU::MSI | CPU::MTI | CPU::MEI);  // enable interrupts at CLINT
    CPU::sp(Traits<Machine>::BOOT_STACK - Traits<Machine>::STACK_SIZE * CPU::id()); // set this hart stack
    CLINT::mtvec(CLINT::DIRECT, _int_entry);
    if(CPU::id() == 0) {
        CPU::mstatus(CPU::MPP_M);               // interrupts are kept disabled for hart 0
        CPU::mepc(CPU::Reg(&setup));
    } else {
        CPU::mstatus(CPU::MPP_M | CPU::MPIE);   // interrupts reenabled to wait for IPI (aka INT_RESCHEDULER)
        CPU::mepc(CPU::Reg(&wait));
    }
    CPU::mret();
}

void wait()
{
    CPU::halt();
    IC::ipi_eoi(IC::INT_RESCHEDULER); // acknowledge the IPI (i.e. clear MIP.MSI) so we don't loop forever here
    setup();
}

void setup()
{
    Setup setup(reinterpret_cast<char *>(Memory_Map::SYS_INFO));
}
