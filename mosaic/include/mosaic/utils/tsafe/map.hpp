#pragma once

#include <unordered_map>
#include <optional>
#include <shared_mutex>

namespace mosaic
{
namespace utils
{
namespace tsafe
{

template <typename K, typename V>
class ThreadSafeMap
{
   private:
    std::unordered_map<K, V> m_map;
    mutable std::shared_mutex m_mutex;

   public:
    ThreadSafeMap() = default;
    ThreadSafeMap(const ThreadSafeMap&) = delete;
    ThreadSafeMap& operator=(const ThreadSafeMap&) = delete;

    void insert(const K& key, const V& value)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map[key] = value;
    }

    void insert(const K& key, V&& value)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map[key] = std::move(value);
    }

    void insertIfAbsent(const K& key, const V& value)
    {
        std::unique_lock lock(m_mutex);
        if (m_map.find(key) == m_map.end())
        {
            m_map[key] = value;
        }
    }

    void insertAll(const std::unordered_map<K, V>& other)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        auto backup = m_map;

        try
        {
            for (const auto& [key, value] : other)
            {
                m_map[key] = value;
            }
        }
        catch (...)
        {
            m_map = std::move(backup);
            throw;
        }
    }

    template <typename Func>
    void transform(Func _func)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        auto backup = m_map;

        try
        {
            _func(m_map);
        }
        catch (...)
        {
            m_map = std::move(backup);
            throw;
        }
    }

    std::optional<V> get(const K& key) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end()) return it->second;
        return std::nullopt;
    }

    bool contains(const K& key) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.find(key) != m_map.end();
    }

    bool erase(const K& key)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        return m_map.erase(key) > 0;
    }

    size_t size() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.size();
    }

    void clear()
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map.clear();
    }
};

} // namespace tsafe
} // namespace utils
} // namespace mosaic
