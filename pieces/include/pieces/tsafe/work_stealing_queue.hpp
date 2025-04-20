#pragma once

#include <deque>
#include <mutex>
#include <optional>

namespace pieces
{
namespace tsafe
{

/**
 * @brief A thread-safe work-stealing queue implementation.
 *
 * This class provides a thread-safe interface for inserting and removing
 * elements from a deque. It uses a mutex to ensure safe access from multiple threads.
 *
 * @tparam T Type of elements in the queue.
 */
template <typename T>
class WorkStealingQueue
{
   private:
    mutable std::mutex m_mutex;
    std::deque<T> m_deque;

   public:
    WorkStealingQueue() = default;
    WorkStealingQueue(const WorkStealingQueue&) = delete;
    WorkStealingQueue& operator=(const WorkStealingQueue&) = delete;

    /**
     * @brief Push an element to the back of the queue.
     *
     * @param _value The value to be pushed into the queue.
     */
    void push(const T& _value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_deque.push_back(_value);
    }

    /**
     * @brief Push a value to the back of the queue using move semantics.
     *
     * @param _value The rvalue reference to be pushed into the queue.
     */
    void push(T&& _value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_deque.push_back(std::move(_value));
    }

    /**
     * @brief Try to pop an element from the back of the queue (LIFO).
     *
     * @param _result The reference to store the popped value.
     * @return true if an item was popped successfully.
     * @return false if the queue was empty.
     */
    bool tryPop(T& _result)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_deque.empty())
        {
            return false;
        }

        _result = std::move(m_deque.back());
        m_deque.pop_back();

        return true;
    }

    /**
     * @brief Try to pop an element from the front of the queue (FIFO).
     *
     * @param _result The reference to store the popped value.
     * @return true if an item was popped successfully.
     * @return false if the queue was empty.
     */
    bool trySteal(T& _result)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_deque.empty())
        {
            return false;
        }

        _result = std::move(m_deque.front());
        m_deque.pop_front();

        return true;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_deque.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_deque.size();
    }
};

} // namespace tsafe
} // namespace pieces
