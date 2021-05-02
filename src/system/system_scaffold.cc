// EPOS System Scaffold and System Component Implementation

#include <utility/ostream.h>
#include <utility/heap.h>
#include <machine.h>
#include <memory.h>
#include <process.h>
#include <system.h>
#include <framework/message.h>

__BEGIN_SYS

// Global objects
// These objects might be reconstructed several times in multicore configurations, so their constructors must be stateless!
OStream kout;
OStream kerr;

// System class attributes
System_Info * System::_si = reinterpret_cast<System_Info *>(Memory_Map::SYS_INFO);
char System::_preheap[];
Segment * System::_heap_segment;
Heap * System::_heap;

__END_SYS

// Bindings
extern "C" {
    __USING_SYS;

    // Libc legacy
    void _panic() { Machine::panic(); }
    void _exit(int s) { Thread::exit(s); for(;;); }
    void __exit() { Thread::exit(CPU::fr()); }  // must be handled by the Page Fault handler for user-level tasks
    void __cxa_pure_virtual() { db<void>(ERR) << "Pure Virtual method called!" << endl; }

    // Utility-related methods that differ from kernel and user space.
    // OStream
    void _print(const char * s) {
        Display::puts(s);     
    }
}

// EPOS Kernel Binding

#include <framework/main.h>
#include <framework/agent.h>

// Framework class attributes
__BEGIN_SYS

Agent::Member Agent::_handlers[] = {&Agent::handle_thread,
                                    &Agent::handle_task,
                                    &Agent::handle_active,
                                    &Agent::handle_address_space,
                                    &Agent::handle_segment,
                                    &Agent::handle_mutex,
                                    &Agent::handle_semaphore,
                                    &Agent::handle_condition,
                                    &Agent::handle_clock,
                                    &Agent::handle_alarm,
                                    &Agent::handle_chronometer,
                                    &Agent::handle_utility
};

__END_SYS

__USING_SYS;
extern "C" { void _exec(void * m) { reinterpret_cast<Agent *>(m)->exec(); } }

