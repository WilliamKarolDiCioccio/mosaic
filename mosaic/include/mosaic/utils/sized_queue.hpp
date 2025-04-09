#pragma once

#include <vector>
#include <stdexcept>

namespace mosaic
{
namespace utils
{

/**
 * @brief A queue with a fixed size that discards the oldest element when full.
 *
 * This class implements a queue that has a fixed size. When the queue is full and a new element is
 * added, the oldest element is removed to make space for the new one. This is useful for scenarios
 * where you want to keep only the most recent elements in memory. This is also known as a
 * circular buffer.
 *
 * @note The class provides trivial wrappers for the STL vector methods, so you can use it both
 * as a queue and as a vector.
 *
 * @tparam T The type of the elements in the queue.
 */
template <typename T>
class SizedQueue
{
   private:
    std::vector<T> m_data;
    std::size_t m_capacity;

   public:
    /**
     * @brief Constructs a SizedQueue with the specified capacity.
     *
     * You won't be able to change capacity after the queue is created.
     *
     * @param _capacity The maximum number of elements the queue can hold.
     * @throws std::invalid_argument if _capacity is 0.
     */
    explicit SizedQueue(std::size_t _capacity) : m_capacity(_capacity)
    {
        if (_capacity == 0)
        {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        m_data.reserve(_capacity);
    }

    /**
     * @brief Adds an element to the front of the queue.
     *
     * @param _value The value to add to the queue.
     */
    void push(const T& _value)
    {
        if (m_data.size() == m_capacity)
        {
            m_data.pop_back();
        }
        m_data.insert(m_data.begin(), _value);
    }

    /**
     * @brief Adds an element to the front of the queue using move semantics.
     *
     * @param _value The rvalue reference to add to the queue.
     */
    void push(T&& _value)
    {
        if (m_data.size() == m_capacity)
        {
            m_data.pop_back();
        }
        m_data.insert(m_data.begin(), std::move(_value));
    }

    /**
     * @brief Removes the oldest element from the queue and returns it.
     *
     * @param _out The variable to store the removed element.
     * @return true if an element was removed, false if the queue was empty.
     */
    bool pop(T& _out)
    {
        if (empty())
        {
            return false;
        }
        _out = std::move(m_data.front());
        m_data.erase(m_data.begin());
        return true;
    }

    /**
     * @brief Removes the oldest element from the queue.
     *
     * @return true if an element was removed, false if the queue was empty.
     * @throws std::out_of_range if the queue is empty.
     */
    const T& front() const
    {
        if (empty())
        {
            throw std::out_of_range("Queue is empty");
        }
        return m_data.front();
    }

    /**
     * @brief Returns the last element in the queue.
     *
     * @return The last element in the queue.
     * @throws std::out_of_range if the queue is empty.
     */
    const T& back() const
    {
        if (empty())
        {
            throw std::out_of_range("Queue is empty");
        }
        return m_data.back();
    }

    bool empty() const { return m_data.empty(); }
    std::size_t size() const { return m_data.size(); }
    std::size_t capacity() const { return m_capacity; }
    void clear() { m_data.clear(); }

    std::vector<T>& data() { return m_data; }
    const std::vector<T>& data() const { return m_data; }

    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }
    auto begin() const { return m_data.begin(); }
    auto end() const { return m_data.end(); }

    /**
     * @brief Access an element at a specific index.
     *
     * @param _index The index of the element to access.
     * @return const T& The element at the specified index.
     * @throws std::out_of_range if the index is out of range or the queue is empty.
     */
    const T& operator[](size_t _index) const
    {
        if (empty())
        {
            throw std::out_of_range("Queue is empty");
        }
        else if (_index >= m_data.size() || _index < 0)
        {
            throw std::out_of_range("Index out of range");
        }

        return m_data[_index];
    }
};

} // namespace utils
} // namespace mosaic
