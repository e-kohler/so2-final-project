// EPOS Application Scaffold and Application Component Implementation

#include <system.h>
#include <framework/message.h>
#include <framework/main.h>

// Framework class attributes
__BEGIN_SYS
Framework::Cache Framework::_cache;
__END_SYS

__BEGIN_SYS

// Application class attributes
char Application::_preheap[];
Heap * Application::_heap;

__END_SYS

__BEGIN_API

// Global objects
__USING_UTIL
OStream cout;
OStream cerr;

__END_API

// Bindings
extern "C" {
    __USING_SYS;

    // Utility-related methods that differ from kernel and user space.
    // OStream
    void _syscall(void * m) { CPU::syscall(m); }
    void _print(const char * s) {
        // Display::puts(s);     
        Message msg(Id(UTILITY_ID, 0), Message::PRINT, reinterpret_cast<unsigned int>(s));
        msg.act();
    }
}
