// EPOS Task Test Program

#include <framework/main.h>

const int iterations = 10;

int func_a(void);
int func_b(void);

EPOS::Thread * a;
EPOS::Thread * b;
EPOS::Thread * m;

EPOS::OStream cout;

int main()
{
    cout << "Task test" << EPOS::S::U::endl;

    m = EPOS::Thread::self();

    EPOS::Task * task0 = EPOS::Task::self();
    EPOS::Address_Space * as0 = task0->address_space();
    cout << "My address space's page directory is located at " << as0->pd() << EPOS::S::U::endl;

    EPOS::Segment * cs0 = task0->code_segment();
    EPOS::CPU::Log_Addr code0 = task0->code();
    cout << "My code segment is located at "
         << static_cast<void *>(code0)
         << " and it is " << cs0->size() << " bytes long" << EPOS::S::U::endl;

    EPOS::Segment * ds0 = task0->data_segment();
    EPOS::CPU::Log_Addr data0 = task0->data();
    cout << "My data segment is located at "
         << static_cast<void *>(data0)
         << " and it is " << ds0->size() << " bytes long" << EPOS::S::U::endl;

    cout << "Creating a thread:";
    a = new EPOS::Thread(&func_a);
    cout << " done!" << EPOS::S::U::endl;

    cout << "Creating another thread:";
    b = new EPOS::Thread(&func_b);
    cout << " done!" << EPOS::S::U::endl;

    cout << "I'll now suspend my self to see the other threads running:" << EPOS::S::U::endl;
    m->suspend();

    cout << "Both threads are now done and have suspended themselves. I'll now wait for 1 second and then wake them up so they can exit ..." << EPOS::S::U::endl;

    EPOS::Alarm::delay(1000000);

    a->resume();

    EPOS::Thread::yield();
    b->resume();

    int status_a = a->join();
    int status_b = b->join();

    cout << "Thread A exited with status " << status_a << " and thread B exited with status " << status_b << "." << EPOS::S::U::endl;

    delete a;
    delete b;

    cout << "I'm also done, bye!" << EPOS::S::U::endl;

    return 0;
}


int func_a(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 79; i++)
            cout << "a";
        cout << EPOS::S::U::endl;
        EPOS::Thread::yield();
    }

    EPOS::Thread::self()->suspend();

    return 'A';
}

int func_b(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 79; i++)
            cout << "b";
        cout << EPOS::S::U::endl;
        EPOS::Thread::yield();
    }

    m->resume();

    EPOS::Thread::self()->suspend();

    return 'B';
}
