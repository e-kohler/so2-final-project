#include <framework/main.h>
#include <utility/ostream.h>

EPOS::OStream cout;
const int iterations = 10;

int thread_func(void);
void alarm_func();

int main()
{
    ///* SOME SYSCALL EXAMPLES (with PRINT) *///

    // THREAD
    {
        cout << "Creating thread A..." << EPOS::S::U::endl;
        EPOS::Thread * threadA = new EPOS::Thread(&thread_func);
        cout << "Thread A done" << EPOS::S::U::endl;

        cout << "Creating thread B..." << EPOS::S::U::endl;
        EPOS::Thread * threadB = new EPOS::Thread(&thread_func);
        cout << "Thread B done" << EPOS::S::U::endl;

        cout << "Suspending ThreadB to see the other thread running:" << EPOS::S::U::endl;
        threadB->suspend();

        threadA->resume();
        // EPOS::Thread::yield();
        threadB->resume();

        int status_thread_a = threadA->join();
        int status_thread_b = threadB->join();

        cout << "Thread A exited with status " << status_thread_a << " and thread B exited with status " << status_thread_b << "." << EPOS::S::U::endl;

        delete threadA;
        delete threadB;
    }
   
    // TASK / ADDRESS SPACE / SEGMENT
    {
        EPOS::Task * task = EPOS::Task::self();

        EPOS::Address_Space * as = task->address_space();
        cout << "My address space's page directory is located at " << as->pd() << EPOS::S::U::endl;
        EPOS::Segment * cs = task->code_segment();
        EPOS::CPU::Log_Addr code0 = task->code();
        cout << "My code segment is located at "
            << static_cast<void *>(code0)
            << " and it is " << cs->size() << " bytes long" << EPOS::S::U::endl;
            
        EPOS::Segment * ds = task->data_segment();
        EPOS::CPU::Log_Addr data0 = task->data();
        cout << "My data segment is located at "
            << static_cast<void *>(data0)
            << " and it is " << ds->size() << " bytes long" << EPOS::S::U::endl;
    }
    
    // MUTEX
    {
        EPOS::Mutex mutex;
        mutex.lock();
        cout << "Locked mutex" << EPOS::S::U::endl;
        mutex.unlock();
        cout << "Unlocked mutex" << EPOS::S::U::endl;
    }
    

    // SEMAPHORE
    {
        EPOS::Semaphore semaphore;
        semaphore.lock();
        cout << "Locked semaphore" << EPOS::S::U::endl;
        semaphore.unlock();
        cout << "Unlocked semaphore" << EPOS::S::U::endl;
    }

    // ALARM
    {
        EPOS::Alarm alarm;
        EPOS::Function_Handler handler_alarm(&alarm_func);
        EPOS::Alarm alarm_a(2000, &handler_alarm, iterations);

        EPOS::Alarm::delay(2000 * (iterations + 2));
    }

    return 0;
}

int thread_func(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 30; i++)
            cout << "Syscall test: Thread!" << EPOS::S::U::endl;
        cout << EPOS::S::U::endl;
        EPOS::Thread::yield();
    }
    
    return 'A';
}

void alarm_func()
{
    for(int i = 0; i < 30; i++)
        cout << "Syscall test: Alarm!" << EPOS::S::U::endl;
    cout << EPOS::S::U::endl;
}
