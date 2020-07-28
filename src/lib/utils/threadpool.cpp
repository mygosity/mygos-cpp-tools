#include "threadpool.h"

namespace CodeBlacksmith
{

    ThreadPool::ThreadPool(std::size_t numThreads) : DynamicObject("ThreadPool")
    {
        Start(numThreads);
    }

    ThreadPool::~ThreadPool()
    {
        Stop();
    }

    void ThreadPool::InvokeMethod(std::string &methodKey)
    {
    }

    void ThreadPool::Start(std::size_t numThreads)
    {
        for (auto i = 0u; i < numThreads; ++i)
        {
            m_Threads.emplace_back([=, this] {
                while (true)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{m_EventMutex};
                        // As in the previous example, you can achieve the same variable-capture goals by using different combinations of capture options. The following list includes different capture options that produce the same result as [divisor, &sum] in the previous example.
                        // [=, &sum]: Captures any referenced variable within the lambda by value (making a copy), except sum that has to be captured by reference.
                        // [&, divisor]: Captures any referenced variable within the lambda by reference, except divisor that has to be captured by value.
                        // https://www.drdobbs.com/cpp/lambdas-in-c11/240168241?pgno=2
                        m_EventVar.wait(lock, [=, this] { return m_Stopping || !m_Tasks.empty(); });
                        if (m_Stopping && m_Tasks.empty())
                        {
                            break;
                        }
                        task = std::move(m_Tasks.front());
                        m_Tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void ThreadPool::Stop() noexcept
    {
        {
            std::unique_lock<std::mutex> lock{m_EventMutex};
            m_Stopping = true;
        }
        m_EventVar.notify_all();
        for (auto &thread : m_Threads)
        {
            thread.join();
        }
    }

} // namespace CodeBlacksmith