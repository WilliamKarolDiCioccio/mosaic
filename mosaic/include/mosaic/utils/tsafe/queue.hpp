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

/**
 * @brief Thread-safe queue wrapper around std::queue.
 *
 * This class provides a thread-safe interface for inserting and removing
 * elements from a queue. It uses a mutex and condition variable to ensure
 * safe access from multiple threads. The object is not copyable or movable.
 *
 * @tparam T Type of elements in the queue.
 */
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

    /**
     * @brief Push an element to the back of the queue.
     *
     * @param _value The value to be pushed into the queue.
     */
    void push(const T& _value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(_value);
        }
        m_condVar.notify_one();
    }

    /**
     * @brief Push a value to the back of the queue using move semantics.
     *
     * @param _value The rvalue reference to be pushed into the queue.
     */
    void push(T&& _value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(_value));
        }
        m_condVar.notify_one();
    }

    /**
     * @brief Wait for an element to be available and pop it from the queue.
     *
     * @param _value The reference to store the popped value.
     */
    void wait_and_pop(T& _value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [this] { return !m_queue.empty(); });
        _value = std::move(m_queue.front());
        m_queue.pop();
    }

    /**
     * @brief Try to pop an element from the queue without blocking.
     *
     * @return std::optional<T> The popped value, or std::nullopt if the queue is empty.
     */
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

    /**
     * @brief Check if the queue is empty.
     *
     * @return true if the queue is empty
     * @return false if the queue is not empty
     */
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    /**
     * @brief Get the number of elements in the queue.
     *
     * @return size_t The number of elements in the queue.
     */
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

} // namespace tsafe
} // namespace utils
} // namespace mosaic
