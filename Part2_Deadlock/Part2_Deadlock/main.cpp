
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

mutex mutexA;
mutex mutexB;

void threadOneScoped()
{
    for (int i = 0; i < 5; i++)
    {
        scoped_lock lock(mutexA, mutexB);

        cout << "Thread 1 (scoped_lock): Working\n";

        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void threadTwoScoped()
{
    for (int i = 0; i < 5; i++)
    {
        scoped_lock lock(mutexB, mutexA);

        cout << "Thread 2 (scoped_lock): Working\n";

        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void threadOneOrdered()
{
    for (int i = 0; i < 5; i++)
    {
        lock_guard<mutex> lockA(mutexA);
        lock_guard<mutex> lockB(mutexB);

        cout << "Thread 1 (ordered): Working\n";

        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void threadTwoOrdered()
{
    for (int i = 0; i < 5; i++)
    {
        lock_guard<mutex> lockA(mutexA);
        lock_guard<mutex> lockB(mutexB);

        cout << "Thread 2 (ordered): Working\n";

        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

int main()
{
    cout << "DEADLOCK PREVENTION\n\n";

    cout << "1. Testing scoped_lock\n";

    thread t1(threadOneScoped);
    thread t2(threadTwoScoped);

    t1.join();
    t2.join();

    cout << "scoped_lock test completed\n\n";

    cout << "2. Testing consistent lock order\n";

    thread t3(threadOneOrdered);
    thread t4(threadTwoOrdered);

    t3.join();
    t4.join();

    cout << "Lock order test completed\n";
    cout << "\nAll tests completed successfully\n";

    return 0;
}
