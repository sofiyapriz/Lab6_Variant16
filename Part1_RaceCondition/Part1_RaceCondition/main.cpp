
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>

using namespace std;

const int ITERATIONS = 1600000;
const int EXPECTED = ITERATIONS * 2;

int counter = 0;
mutex counterMutex;
atomic<int> atomicCounter(0);

void incrementWithoutSync()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter++;
    }
}

void incrementWithMutex()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        lock_guard<mutex> lock(counterMutex);
        counter++;
    }
}

void incrementWithAtomic()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        atomicCounter.fetch_add(1, memory_order_relaxed);
    }
}

template <typename Function>
double measureExecution(Function function)
{
    auto start = chrono::steady_clock::now();

    thread t1(function);
    thread t2(function);

    t1.join();
    t2.join();

    auto end = chrono::steady_clock::now();

    return chrono::duration<double, milli>(end - start).count();
}

int main()
{
    cout << fixed << setprecision(3);
    cout << "RACE CONDITION TEST - VARIANT 16\n";
    cout << "Iterations per thread: " << ITERATIONS << "\n";
    cout << "Expected result: " << EXPECTED << "\n\n";

    int incorrectRuns = 0;

    cout << "1. WITHOUT SYNCHRONIZATION\n";

    for (int run = 1; run <= 10; run++)
    {
        counter = 0;

        double time = measureExecution(incrementWithoutSync);

        cout << "Run " << run
            << ": Expected = " << EXPECTED
            << ", Actual = " << counter
            << ", Time = " << time << " ms";

        if (counter != EXPECTED)
        {
            incorrectRuns++;
            cout << " - INCORRECT";
        }

        cout << "\n";
    }

    cout << "Incorrect runs: " << incorrectRuns << " / 10\n\n";

    cout << "2. WITH MUTEX\n";

    double mutexTotal = 0;

    for (int run = 1; run <= 5; run++)
    {
        counter = 0;

        double time = measureExecution(incrementWithMutex);
        mutexTotal += time;

        cout << "Run " << run
            << ": Result = " << counter
            << ", Time = " << time << " ms\n";
    }

    cout << "Average mutex time: " << mutexTotal / 5 << " ms\n\n";

    cout << "3. WITH ATOMIC\n";

    double atomicTotal = 0;

    for (int run = 1; run <= 5; run++)
    {
        atomicCounter.store(0);

        double time = measureExecution(incrementWithAtomic);
        atomicTotal += time;

        cout << "Run " << run
            << ": Result = " << atomicCounter.load()
            << ", Time = " << time << " ms\n";
    }

    cout << "Average atomic time: " << atomicTotal / 5 << " ms\n";

    cout << "\nTEST COMPLETED\n";

    return 0;
}
