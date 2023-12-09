#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace worm::detail {
class ThreadPool {
public:
    inline ThreadPool(const size_t threads)
        : m_running{ true }
    {
        for (size_t i = 0; i < threads; ++i)
            m_workers.emplace_back(
                [this] {
                    for (;;) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock{ this->m_queueMutex };

                            this->m_runningCondition.wait(lock,
                                [this] {
                                    return !this->m_running || !this->m_tasks.empty();
                                });

                            if (!this->m_running && this->m_tasks.empty()) {
                                break;
                            }

                            task = std::move(this->m_tasks.front());

                            this->m_tasks.pop();
                        }

                        task();
                    }
                });
    }

    inline ~ThreadPool()
    {
        {
            std::scoped_lock<std::mutex> lock{ m_queueMutex };
            m_running = false;
        }

        m_runningCondition.notify_all();

        for (auto& worker : m_workers) {
            worker.join();
        }
    }

public:
    template <class F, class... Args>
    decltype(auto) Enqueue(F&& f, Args&&... args)
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::scoped_lock<std::mutex> lock{ m_queueMutex };

            if (!m_running) {
                throw std::runtime_error("Enqueue on not running ThreadPool");
            }

            m_tasks.emplace([task]() {
                (*task)();
            });
        }
        m_runningCondition.notify_one();
        return res;
    }

private:
    std::atomic<bool> m_running;

    std::vector<std::thread> m_workers;

    std::queue<std::function<void()>> m_tasks;

    std::mutex m_queueMutex;

    std::condition_variable m_runningCondition;
};
} // namespace worm::detail

#endif
