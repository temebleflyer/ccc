#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>
#include <type_traits>
#include <memory>

using namespace std;

class ThreadPool
{
public:
    // create thread
    explicit ThreadPool(size_t threadCount = thread::hardware_concurrency())
    : m_stopflag(false) {
        for (size_t i = 0; i < threadCount; i++)
        {
            m_workers.emplace_back([this] {
                while (true)
                {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(m_queuemutex);
                        m_condition.wait(lock, [this] {
                            return m_stopflag || !m_taskqueue.empty();
                        });

                        if (m_stopflag && m_taskqueue.empty())
                        {
                            return;
                        }

                        task = move(m_taskqueue.front());
                        m_taskqueue.pop();
                    }
                    task();
                }
            });
        }
    }

    //submit task
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> future<typename result_of<F(Args...)>::type> {
            using return_type = typename result_of<F(Args...)>::type;

            auto task = make_shared<packaged_task<return_type()>>(
                bind(forward<F>(f), forward<Args>(args)...)
            );

            future<return_type> result = task->get_future();
            {
                

                unique_lock<mutex> lock(m_queuemutex);
                if (m_stopflag)
                {
                    throw runtime_error("enqueue on stop threadpool");
                }
                m_taskqueue.emplace([task]() { (*task)(); });
            }
            m_condition.notify_one();
            return result;
    }

    ~ThreadPool()
    {
        {
            unique_lock<mutex> lock(m_queuemutex);
            m_stopflag = true;
        }
        m_condition.notify_all();
        for (thread &worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    // get task count
    size_t getTaskCount()
    {
        unique_lock<mutex> lock(m_queuemutex);
        return m_taskqueue.size();
    }
private:
    vector<thread> m_workers;
    condition_variable m_condition;
    queue<function<void()>> m_taskqueue;
    int m_phtreadcount;
    bool m_stopflag;
    mutex m_queuemutex;
};

#endif  //THREADPOOL_H