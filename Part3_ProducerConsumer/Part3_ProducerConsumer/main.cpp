
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

using namespace std;

struct Notification
{
    int id;
    int priority;
    string category;
    string message;
};

struct ComparePriority
{
    bool operator()(const Notification& a, const Notification& b) const
    {
        return a.priority < b.priority;
    }
};

priority_queue<Notification, vector<Notification>, ComparePriority> notifications;

mutex queueMutex;
mutex outputMutex;
condition_variable condition;

bool finished = false;
atomic<int> processed(0);
int maxQueueSize = 0;

vector<Notification> createNotifications()
{
    return {
        {1, 2, "Information", "System update available"},
        {2, 5, "Critical", "Server connection lost"},
        {3, 1, "General", "New user registered"},
        {4, 4, "Warning", "Disk space is running low"},
        {5, 3, "Information", "Backup completed"},
        {6, 5, "Critical", "Database failure detected"},
        {7, 2, "General", "New message received"},
        {8, 4, "Warning", "High memory usage"},
        {9, 1, "General", "User logged out"},
        {10, 3, "Information", "Report generated"},
        {11, 5, "Critical", "Security alert detected"},
        {12, 2, "Information", "Scheduled task completed"}
    };
}

void producer()
{
    vector<Notification> data = createNotifications();

    for (const auto& notification : data)
    {
        {
            lock_guard<mutex> lock(queueMutex);
            notifications.push(notification);

            if (static_cast<int>(notifications.size()) > maxQueueSize)
            {
                maxQueueSize = static_cast<int>(notifications.size());
            }
        }

        condition.notify_one();
        this_thread::sleep_for(chrono::milliseconds(30));
    }

    {
        lock_guard<mutex> lock(queueMutex);
        finished = true;
    }

    
    condition.notify_all();
}
void consumer(int id)
{
    while (true)
    {
        Notification current;

        {
            unique_lock<mutex> lock(queueMutex);

            condition.wait(lock, [] {
                return !notifications.empty() || finished;
                });

            if (notifications.empty() && finished)
            {
                return;
            }

            current = notifications.top();
            notifications.pop();
        }

        {
            lock_guard<mutex> lock(outputMutex);
            cout << "Consumer " << id
                << " processed notification " << current.id
                << " (Priority: " << current.priority << ")\n";
        }

        this_thread::sleep_for(chrono::milliseconds(500));
        processed++;
    }
}

void runTest(int consumerCount)
{
    finished = false;
    processed = 0;
    maxQueueSize = 0;

    while (!notifications.empty())
    {
        notifications.pop();
    }

    cout << "\n====================================\n";
    cout << "Testing with " << consumerCount << " consumer(s)\n";
    cout << "====================================\n";

    auto start = chrono::steady_clock::now();

    thread producerThread(producer);

    vector<thread> consumers;

    for (int i = 1; i <= consumerCount; i++)
    {
        consumers.emplace_back(consumer, i);
    }

    producerThread.join();

    for (auto& consumerThread : consumers)
    {
        consumerThread.join();
    }

    auto end = chrono::steady_clock::now();

    double elapsed = chrono::duration<double, milli>(end - start).count();

    cout << "\nProcessed notifications: " << processed << "\n";
    cout << "Maximum queue size: " << maxQueueSize << "\n";
    cout << "Execution time: " << elapsed << " ms\n";
}

int main()
{
    cout << "PRODUCER-CONSUMER COMPARISON - VARIANT 16\n";

    runTest(1);
    runTest(2);
    runTest(4);

    cout << "\nALL TESTS COMPLETED\n";

    return 0;
}
