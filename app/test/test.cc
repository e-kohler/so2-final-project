#include <utility/ostream.h>

const int iterations = 10;

int thread_func(void);
void alarm_func();

using namespace EPOS;
OStream cout;

int main()
{
    ///* SOME SYSCALL EXAMPLES (with PRINT) *///

        // TASK / ADDRESS SPACE / SEGMENT
    {
        Task * task = Task::self();

        Address_Space * as = task->address_space();
        cout << "My address space's page directory is located at " << as->pd() << endl;
        Segment * cs = task->code_segment();
        CPU::Log_Addr code0 = task->code();
        cout << "My code segment is located at "
            << static_cast<void *>(code0)
            << " and it is " << cs->size() << " bytes long" << endl;
            
        Segment * ds = task->data_segment();
        CPU::Log_Addr data0 = task->data();
        cout << "My data segment is located at "
            << static_cast<void *>(data0)
            << " and it is " << ds->size() << " bytes long" << endl;
    }

    // THREAD
    {
        cout << "Creating thread A..." << endl;
        Thread * threadA = new Thread(&thread_func);
        cout << "Thread A done" << endl;

        cout << "Creating thread B..." << endl;
        Thread * threadB = new Thread(&thread_func);
        cout << "Thread B done" << endl;

        cout << "Suspending ThreadA to see the other thread running:" << endl;
        threadA->suspend();

        cout << "Suspending ThreadB to see the other thread running:" << endl;
        threadB->suspend();

        cout << "Both threads are now done and have suspended themselves. I'll now wait for 1 second and then wake them up so they can exit ..." << endl;
        Alarm::delay(1000000);

        threadA->resume();
        Thread::yield();
        threadB->resume();

        int status_thread_a = threadA->join();
        int status_thread_b = threadB->join();

        cout << "Thread A exited with status " << status_thread_a << " and thread B exited with status " << status_thread_b << "." << endl;

        delete threadA;
        delete threadB;
    }
    
    // MUTEX
    {
        Mutex mutex;
        mutex.lock();
        cout << "Locked mutex" << endl;
        mutex.unlock();
        cout << "Unlocked mutex" << endl;
    }
    

    // // SEMAPHORE
    {
        Semaphore semaphore;
        semaphore.p();
        cout << "Locked semaphore" << endl;
        semaphore.v();
        cout << "Unlocked semaphore" << endl;
    }

    // ALARM
    {
        Alarm::delay(2000 * (iterations + 2));
    }

    return 0;
}

int thread_func(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 30; i++)
            cout << "Syscall test: Thread!" << endl;
        cout << endl;
        Thread::yield();
    }
    
    return 'A';
}