#pragma once

#include <array>
#include <vector>
#include <memory>
#include <type_traits>

#include "pieces/internal/error_codes.hpp"
#include "pieces/result.hpp"

namespace pieces
{

/**
 * @brief A sparse set implementation that allows for efficient storage and retrieval of key-value
 * pairs, where keys are non-negative integers.
 *
 * Sparse sets offer O1 complexity for insertion, deletion, and lookup operations while paying a
 * price in memory usage. To mitigate this, the implementation uses a page-based approach to store
 * keys and values, allowing for more efficient memory management. This means that the more sparse
 * the set is the more often you'll pay a small price for the allocation of new pages.
 *
 * @tparam K The type of the keys (must be an unsigned integral type).
 * @tparam T The type of the values.
 * @tparam PageSize The size of each page (default is 64). This should be a positive integer.
 */
template <typename K, typename T, int PageSize = 64>
    requires(std::is_integral_v<K> && std::is_unsigned_v<K> && PageSize > 0)
class SparseSet
{
   private:
    struct Page
    {
        std::array<size_t, PageSize> sparse{}; // maps key index to dense index
        std::array<bool, PageSize> present{};  // whether key index is present

        Page() { present.fill(false); } // Initialize all keys as not present
    };

   public:
    SparseSet() = default;

    /**
     * @brief Inserts a key-value pair into the sparse set. If the key already exists, it updates
     * the value.
     *
     * @param _key The key to insert.
     * @param _value The value to associate with the key.
     */
    void insert(K _key, const T& _value)
    {
        Page& page = ensurePageExists(_key + 1);

        if (!page.present[pageOffset(_key)])
        {
            // new key
            page.sparse[pageOffset(_key)] = m_denseKeys.size();
            m_denseKeys.push_back(_key);
            m_data.push_back(_value);
            page.present[pageOffset(_key)] = true;
        }
        else
        {
            // overwrite existing
            m_data[page.sparse[pageOffset(_key)]] = _value;
        }
    }

    /**
     * @brief Inserts a key-value pair into the sparse set using move semantics. If the key already
     * exists, it updates the value.
     *
     * @param _key The key to insert.
     * @param _value The value to associate with the key.
     */
    void emplace(K _key, T&& _value)
    {
        Page& page = ensurePageExists(_key + 1);

        if (!page.present[pageOffset(_key)])
        {
            // new key
            page.sparse[pageOffset(_key)] = m_denseKeys.size();
            m_denseKeys.push_back(_key);
            m_data.emplace_back(std::move(_value));
            page.present[pageOffset(_key)] = true;
        }
        else
        {
            // overwrite existing
            m_data[page.sparse[pageOffset(_key)]] = std::move(_value);
        }
    }

    /**
     * @brief Removes a key-value pair from the sparse set.
     *
     * @param _key The key to remove.
     */
    void remove(K _key)
    {
        size_t pageIdx = pageIndex(_key);

        if (pageIdx >= m_pages.size() || !m_pages[pageIdx]) return;

        Page& page = *m_pages[pageIdx];

        size_t denseIdx = page.sparse[pageOffset(_key)];
        size_t last = m_denseKeys.size() - 1;

        if (denseIdx != last)
        {
            // swap last element into this slot
            const K swapKey = m_denseKeys[last];

            std::swap(m_denseKeys[denseIdx], m_denseKeys[last]);
            std::swap(m_data[denseIdx], m_data[last]);
            page.sparse[pageOffset(swapKey)] = denseIdx;
        }

        // remove last
        m_denseKeys.pop_back();
        m_data.pop_back();
        page.present[pageOffset(_key)] = false;
    }

    /**
     * @brief Checks if a key exists in the sparse set.
     *
     * @param key The key to check.
     * @return true if the key exists, false otherwise.
     */
    bool contains(K _key) const
    {
        size_t pageIdx = pageIndex(_key);

        if (pageIdx >= m_pages.size() || !m_pages[pageIdx]) return false;

        const Page& page = *m_pages[pageIdx];

        return page.present[pageOffset(_key)];
    }

    /**
     * @brief Retrieves the value associated with a key. Returns an error if the key is not found.
     *
     * @param _key The key to retrieve the value for.
     * @return RefResult<T, ErrorCode> The result containing the value or an error code.
     *
     * @see ErrorCode for possible error codes.
     */
    RefResult<T, ErrorCode> get(K _key)
    {
        const size_t pageIdx = pageIndex(_key);

        if (pageIdx >= m_pages.size())
        {
            return ErrRef<T, ErrorCode>(ErrorCode::out_of_range);
        }
        else if (!m_pages[pageIdx])
        {
            return ErrRef<T, ErrorCode>(ErrorCode::key_not_found);
        }

        Page& page = *m_pages[pageIdx];

        if (!page.present[pageOffset(_key)])
        {
            return ErrRef<T, ErrorCode>(ErrorCode::key_not_found);
        }

        return OkRef<T, ErrorCode>(m_data[page.sparse[pageOffset(_key)]]);
    }

    /**
     * @brief Retrieves the value associated with a key. Returns an error if the key is not found.
     *
     * @param _key The key to retrieve the value for.
     * @return RefResult<const T, ErrorCode> The result containing the value or an error code.
     *
     * @see ErrorCode for possible error codes.
     */
    RefResult<const T, ErrorCode> get(K _key) const
    {
        const size_t pageIdx = pageIndex(_key);

        if (pageIdx >= m_pages.size())
        {
            return ErrRef<T, ErrorCode>(ErrorCode::out_of_range);
        }
        else if (!m_pages[pageIdx])
        {
            return ErrRef<T, ErrorCode>(ErrorCode::key_not_found);
        }

        const Page& page = *m_pages[pageIdx];

        if (!page.present[pageOffset(_key)])
        {
            return ErrRef<const T, ErrorCode>(ErrorCode::key_not_found);
        }

        return OkRef<const T, ErrorCode>(m_data[page.sparse[pageOffset(_key)]]);
    }

    size_t size() const noexcept { return m_denseKeys.size(); }

    bool empty() const noexcept { return m_denseKeys.empty(); }

    void clear() noexcept
    {
        m_denseKeys.clear();
        m_data.clear();
        m_pages.clear();
    }

    /**
     * @brief Pre-allocates memory for the sparse set to accommodate a maximum keys and values
     * number (improves performance if know in advance).
     *
     * @param _maxKey The maximum key value to reserve space for.
     * @param _count The number of values to reserve space for.
     */
    void reserve(size_t _maxKey, size_t _count)
    {
        const size_t availablePages = m_pages.size();
        const size_t requiredPages = (_maxKey + PageSize - 1) / PageSize;

        if (availablePages < requiredPages) m_pages.reserve(requiredPages);

        const size_t pagesToAllocate = requiredPages - availablePages;

        for (size_t i = 0; i < pagesToAllocate; ++i)
        {
            m_pages.emplace_back(std::make_unique<Page>());
        }

        m_denseKeys.reserve(_count);
        m_data.reserve(_count);
    }

    const std::vector<K>& keys() const noexcept { return m_denseKeys; }
    std::vector<T>& values() noexcept { return m_data; }
    const std::vector<T>& values() const noexcept { return m_data; }

   private:
    // Get the corresponding page for the given key.
    inline size_t pageIndex(K _key) const { return _key / PageSize; }

    // Get the offset within the page for the given key.
    inline size_t pageOffset(K _key) const { return _key % PageSize; }

    // Ensure the page corresponding to the key exists.
    Page& ensurePageExists(size_t _size)
    {
        const size_t pageIdx = pageIndex(_size);

        if (pageIdx + 1 > m_pages.size()) m_pages.resize(pageIdx + 1);

        if (!m_pages[pageIdx]) m_pages[pageIdx] = std::make_unique<Page>();

        return *m_pages[pageIdx];
    }

    std::vector<std::unique_ptr<Page>> m_pages{}; // stores pages of sparse data
    std::vector<K> m_denseKeys{};                 // stores keys densely
    std::vector<T> m_data{};                      // stores values densely
};

} // namespace pieces
