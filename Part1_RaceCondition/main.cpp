
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

const int ITERATIONS = 1600000;

int counter = 0;

void incrementCounter()
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter++;
    }
}

int main()
{
    for (int run = 1; run <= 10; run++)
    {
        counter = 0;

        auto start = chrono::steady_clock::now();

        thread t1(incrementCounter);
        thread t2(incrementCounter);

        t1.join();
        t2.join();

        auto end = chrono::steady_clock::now();

        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        cout << "Run " << run << ": ";
        cout << "Expected = " << ITERATIONS * 2;
        cout << ", Actual = " << counter;
        cout << ", Time = " << duration.count() << " ms";

        if (counter != ITERATIONS * 2)
        {
            cout << " - RACE CONDITION DETECTED";
        }

        cout << endl;
    }

    return 0;
}
