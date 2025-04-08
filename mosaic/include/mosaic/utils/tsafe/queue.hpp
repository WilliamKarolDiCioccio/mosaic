#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>

namespace mosaic
{
namespace utils
{
namespace tsafe
{

template <typename T>
class ThreadSafeQueue
{
   private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_condVar;

   public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(const T& value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(value);
        }
        m_condVar.notify_one();
    }

    void push(T&& value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(value));
        }
        m_condVar.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [this] { return !m_queue.empty(); });
        value = std::move(m_queue.front());
        m_queue.pop();
    }

    std::optional<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
        {
            return std::nullopt;
        }
        T value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

} // namespace tsafe
} // namespace utils
} // namespace mosaic
