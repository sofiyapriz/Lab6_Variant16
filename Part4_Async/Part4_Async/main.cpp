
#include <iostream>
#include <vector>
#include <future>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

long double sequentialSum(int n, long double x)
{
    long double term = 1.0L;
    long double sum = term;

    for (int k = 1; k < n; k++)
    {
        term *= x / k;
        sum += term;
    }

    return sum;
}

long double partialSum(int start, int end, long double x)
{
    long double term = 1.0L;

    for (int k = 1; k <= start; k++)
    {
        term *= x / k;
    }

    long double sum = term;

    for (int k = start + 1; k < end; k++)
    {
        term *= x / k;
        sum += term;
    }

    return sum;
}

double measureTime(chrono::steady_clock::time_point start,
    chrono::steady_clock::time_point end)
{
    return chrono::duration<double, milli>(end - start).count();
}

void runTest(int n, int taskCount, long double x)
{
    auto sequentialStart = chrono::steady_clock::now();

    long double sequentialResult = sequentialSum(n, x);

    auto sequentialEnd = chrono::steady_clock::now();

    double sequentialTime = measureTime(sequentialStart, sequentialEnd);

    vector<future<long double>> futures;

    auto parallelStart = chrono::steady_clock::now();

    for (int i = 0; i < taskCount; i++)
    {
        int start = i * n / taskCount;
        int end = (i + 1) * n / taskCount;

        futures.push_back(async(launch::async, partialSum, start, end, x));
    }

    long double parallelResult = 0.0L;

    for (auto& future : futures)
    {
        parallelResult += future.get();
    }

    auto parallelEnd = chrono::steady_clock::now();

    double parallelTime = measureTime(parallelStart, parallelEnd);

    double speedup = sequentialTime / parallelTime;

    bool resultsMatch = abs(sequentialResult - parallelResult) < 1e-12L;

    cout << "\nNumber of terms: " << n << "\n";
    cout << "Number of tasks: " << taskCount << "\n";

    cout << setprecision(15);
    cout << "Sequential result: " << sequentialResult << "\n";
    cout << "Parallel result: " << parallelResult << "\n";

    cout << fixed << setprecision(3);
    cout << "Sequential time: " << sequentialTime << " ms\n";
    cout << "Parallel time: " << parallelTime << " ms\n";
    cout << "Speedup: " << speedup << "x\n";
    cout << "Results match: " << (resultsMatch ? "YES" : "NO") << "\n";
}

int main()
{
    const long double x = 1.0L;

    vector<int> sizes = { 1000, 10000, 50000 };
    vector<int> taskCounts = { 2, 4, 8 };

    cout << "ASYNC AND FUTURE TEST - VARIANT 16\n";
    cout << "Calculating exponential series\n";
    cout << "Value of x: " << static_cast<double>(x) << "\n";
    cout << "Expected result (e^1): " << exp(1.0) << "\n";

    for (int n : sizes)
    {
        for (int tasks : taskCounts)
        {
            runTest(n, tasks, x);
        }
    }

    cout << "\nALL TESTS COMPLETED\n";

    return 0;
}
