// EPOS Thread Initialization

#include <machine/timer.h>
#include <machine/ic.h>
#include <system.h>
#include <process.h>

__BEGIN_SYS

extern "C" { void __epos_app_entry(); }

void Thread::init()
{
    db<Init, Thread>(TRC) << "Thread::init()" << endl;

    typedef int (* Main)(int argc, char * argv[]);

    System_Info * si = System::info();

    if(Traits<System>::multitask) {
        new (SYSTEM) Task(new (SYSTEM) Address_Space(MMU::current()),
                              new (SYSTEM) Segment(Log_Addr(si->lm.app_code), si->lm.app_code_size, Segment::Flags::APP),
                              new (SYSTEM) Segment(Log_Addr(si->lm.app_data), si->lm.app_data_size, Segment::Flags::APP),
                              reinterpret_cast<Main>(si->lm.app_entry),
                              Log_Addr(Memory_Map::APP_CODE), Log_Addr(Memory_Map::APP_DATA),
                              static_cast<int>(si->lm.app_extra_size), reinterpret_cast<char **>(si->lm.app_extra));

        if(si->lm.has_ext)
            db<Init>(INF) << "Thread::init: additional data from mkbi at "  << reinterpret_cast<void *>(si->lm.app_extra) << ":" << si->lm.app_extra_size << endl;
    } else
        // If EPOS is a library, then adjust the application entry point to __epos_app_entry,
        // which will directly call main(). In this case, _init will have already been called,
        // before Init_Application to construct MAIN's global objects.
        new (SYSTEM) Thread(Thread::RUNNING, Thread::MAIN, reinterpret_cast<int (*)()>(__epos_app_entry));

    db<Init, Thread>(TRC) << "Thread::init() first task created!" << endl;

    // Idle thread creation does not cause rescheduling (see Thread::constructor_epilogue)
    new (SYSTEM) Thread(Thread::READY, Thread::IDLE, &Thread::idle);

    db<Init, Thread>(TRC) << "Thread::init() thread READY created!" << endl;

    // The installation of the scheduler timer handler does not need to be done after the
    // creation of threads, since the constructor won't call reschedule() which won't call
    // dispatch that could call timer->reset()
    // Letting reschedule() happen during thread creation is also harmless, since MAIN is
    // created first and dispatch won't replace it nor by itself neither by IDLE (which
    // has a lower priority)
    if(Criterion::timed)
        _timer = new (SYSTEM) Scheduler_Timer(QUANTUM, time_slicer);

    // No more interrupts until we reach init_first
    CPU::int_disable();

    // Transition from CPU-based locking to thread-based locking
    This_Thread::not_booting();

    db<Init, Thread>(TRC) << "Thread::init() over!" << endl;
}

__END_SYS
