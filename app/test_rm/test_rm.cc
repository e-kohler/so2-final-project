#include <utility/ostream.h>
#include <time.h>
#include <real-time.h>

using namespace EPOS;

OStream cout;

const unsigned int period_a = 100;
const unsigned int period_b = 200;
const unsigned int period_c = 300;

const unsigned int exec_time_a = 10;
const unsigned int exec_time_b = 10;
const unsigned int exec_time_c = 10;

Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;

const unsigned int iterations = 10;
Chronometer chrono;

void run(char thread, unsigned int time)
{
    Microsecond elapsed = chrono.read() / 1000;
    cout << "\n" << "Thread " << thread << " started at:" << "\t" << elapsed << "ms\t";

    unsigned int final = elapsed + time;
    while (chrono.read() / 1000 < final) {}

    cout << "\n" << "Thread " << thread << " completed at:" << "\t" << chrono.read() / 1000 << "ms\t";
}

int job_a() {
    run('a', exec_time_a);
    while (Periodic_Thread::wait_next()) {
        run('a', exec_time_a);
    };

    return 0;
}

int job_b() {
    run('b', exec_time_b);
    while (Periodic_Thread::wait_next()) {
        run('b', exec_time_b);
    };

    return 0;
}

int job_c() {
    run('c', exec_time_c);
    while (Periodic_Thread::wait_next()) {
        run('c', exec_time_c);
    };

    return 0;
}

 int main()
 {

    thread_a = new Periodic_Thread(period_a * 1000, iterations, &job_a);
    thread_b = new Periodic_Thread(period_b * 1000, iterations, &job_b);
    thread_c = new Periodic_Thread(period_c * 1000, iterations, &job_c);

    chrono.start();

    thread_a->join();
    thread_b->join();
    thread_c->join();

    chrono.stop();

    return 0;
 }