#pragma once

#include "mosaic/defines.hpp"

#include <future>
#include <functional>
#include <memory>
#include <chrono>

#include <pieces/core/result.hpp>
#include <pieces/utils/enum_flags.hpp>

#include "mosaic/core/logger.hpp"

namespace mosaic
{
namespace exec
{

/**
 * @brief Describes how a worker shares or isolates its workload.
 *
 * Each flag enables or restricts specific scheduling behaviors.
 * These can be combined to form more complex policies.
 */
enum class WorkerSharingMode : uint8_t
{
    none = 0,
    allow_steal = 1 << 1,     /// Other workers can steal tasks from it.
    accept_direct = 1 << 3,   /// The thread pool can assign tasks directly to this worker.
    accept_indirect = 1 << 2, /// The thread pool can assign tasks through automatic dispatch.
    global_consumer = 1 << 6, /// Can pull tasks directly from the global queue.
};

MOSAIC_DEFINE_ENUM_FLAGS_OPERATORS(WorkerSharingMode)

/// Common presets for worker behavior.
namespace worker_sharing_presets
{

inline constexpr WorkerSharingMode exclusive = WorkerSharingMode::accept_direct;

inline constexpr WorkerSharingMode shared =
    WorkerSharingMode::allow_steal | WorkerSharingMode::accept_direct |
    WorkerSharingMode::accept_indirect | WorkerSharingMode::global_consumer;

inline constexpr WorkerSharingMode shared_no_steal = WorkerSharingMode::accept_direct |
                                                     WorkerSharingMode::accept_indirect |
                                                     WorkerSharingMode::global_consumer;

} // namespace worker_sharing_presets

/**
 * @brief A thread worker wraps a single thread and manages its task queue.
 *
 * Workers can have different sharing modes that dictate how they interact with the task system.
 *
 * @see WorkerSharingMode
 */
class ThreadWorker;

/**
 * @brief A thread pool manages a collection of worker threads for concurrent task execution.
 *
 * It provides methods to enqueue tasks to either the global queue or specific workers. It creates a
 * worker for each logical CPU core. It allows setting CPU affinity for workers to optimize
 * performance by reducing migration between cores and improving cache locality.
 *
 * @see ThreadWorker
 */
class MOSAIC_API ThreadPool
{
   private:
    static ThreadPool* g_instance;

    struct Impl;

    Impl* m_impl = nullptr;

    friend class ThreadWorker;

   public:
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

   public:
    pieces::RefResult<ThreadPool, std::string> initialize() noexcept;
    void shutdown() noexcept;

    /**
     * @brief Enqueues a task to the global task queue.
     *
     * @tparam F The type of the callable to execute.
     * @tparam Args The types of the arguments to forward to the task.
     * @param f The callable to execute.
     * @param args The arguments to forward to the task.
     * @return std::optional<std::future<std::invoke_result_t<std::decay_t<F>,
     * std::decay_t<Args>...>>>
     *
     * @see assignTaskToGlobal for details on task assignment behavior.
     */
    template <typename F, typename... Args>
    std::optional<std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>>
    enqueueToGlobal(F&& f, Args&&... args)
    {
        using Ret = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto [wrapper, future] = makeTask(std::forward<F>(f), std::forward<Args>(args)...);

        if (!assignTaskToGlobal(std::move(wrapper))) return std::nullopt;

        return std::make_optional(std::move(future));
    }

    /**
     * @brief Tries to perform optimal assignment of a task to worker threads and fallbacks to the
     * global queue if no suitable worker is found.
     *
     * @tparam F The type of the callable to execute.
     * @tparam Args The types of the arguments to forward to the task.
     * @param _f The callable to execute.
     * @param _args The arguments to forward to the task.
     * @return std::optional<std::future<std::invoke_result_t<std::decay_t<F>,
     * std::decay_t<Args>...>>>
     *
     * @see assignTaskToWorker for details on task assignment behavior.
     */
    template <typename F, typename... Args>
    std::optional<std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>>
    enqueueToWorker(F&& _f, Args&&... _args)
    {
        using Ret = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto [wrapper, future] = makeTask(std::forward<F>(_f), std::forward<Args>(_args)...);

        if (!assignTaskToWorker(std::move(wrapper))) return std::nullopt;

        return std::make_optional(std::move(future));
    }

    /**
     * @brief Enqueues a task to a worker thread by its ID.
     *
     * @tparam F The type of the callable to execute.
     * @tparam Args The types of the arguments to forward to the task.
     * @param _id The ID of the worker thread.
     * @param _f The callable to execute.
     * @param _args The arguments to forward to the task.
     * @return std::optional<std::future<std::invoke_result_t<std::decay_t<F>,
     * std::decay_t<Args>...>>>
     *
     * @see assignTaskToWorkerById for details on task assignment behavior.
     */
    template <typename F, typename... Args>
    std::optional<std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>>
    enqueueToWorkerById(uint16_t _id, F&& _f, Args&&... _args)
    {
        using Ret = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto [wrapper, future] = makeTask(std::forward<F>(_f), std::forward<Args>(_args)...);

        if (!assignTaskToWorkerById(_id, std::move(wrapper))) return std::nullopt;

        return std::make_optional(std::move(future));
    }

    /**
     * @brief Enqueues a task to a worker thread by its debug name.
     *
     * @tparam F The type of the callable to execute.
     * @tparam Args The types of the arguments to forward to the task.
     * @param _debugName The debug name of the worker thread.
     * @param _f The callable to execute.
     * @param _args The arguments to forward to the task.
     * @return std::optional<std::future<std::invoke_result_t<std::decay_t<F>,
     * std::decay_t<Args>...>>>
     *
     * @see assignTaskToWorkerByDebugName for details on task assignment behavior.
     */
    template <typename F, typename... Args>
    std::optional<std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>>
    enqueueToWorkerByDebugName(const std::string& _debugName, F&& _f, Args&&... _args)
    {
        using Ret = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto [wrapper, future] = makeTask(std::forward<F>(_f), std::forward<Args>(_args)...);

        if (!assignTaskToWorkerByDebugName(_debugName, std::move(wrapper))) return std::nullopt;

        return std::make_optional(std::move(future));
    }

    void setWorkerAffinity(uint16_t _workerId, size_t _cpuCoreId) noexcept;
    void setWorkerSharingMode(uint16_t _workerId, WorkerSharingMode _sharingMode) noexcept;

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] uint16_t getWorkersCount() const noexcept;
    [[nodiscard]] uint16_t getBusyWorkersCount() const noexcept;
    [[nodiscard]] uint16_t getIdleWorkersCount() const noexcept;

    /// @brief Get a random worker from the pool using a uniform distribution.
    [[nodiscard]] ThreadWorker* getRandomWorker() const noexcept;

    /// @brief Get a worker by its unique ID.
    [[nodiscard]] ThreadWorker* getWorkerByIdx(uint16_t _idx) const noexcept;

    /// @brief Get a worker by its debug name.
    [[nodiscard]] ThreadWorker* getWorkerByDebugName(const std::string& _debugName) const noexcept;

    [[nodiscard]] static inline ThreadPool* getGlobalInstance()
    {
        if (!g_instance) MOSAIC_ERROR("ThreadPool has not been created yet!");

        return g_instance;
    }

   private:
    template <typename F, typename... Args>
    inline auto makeTask(F&& f, Args&&... args)
    {
        using Ret = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto promise = std::make_shared<std::promise<Ret>>();
        std::future<Ret> future = promise->get_future();

        std::move_only_function<void()> wrapper =
            [promise, f = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable
        {
            try
            {
                if constexpr (std::is_void_v<Ret>)
                {
                    std::invoke(std::move(f), std::move(args)...);
                    promise->set_value();
                }
                else
                {
                    promise->set_value(std::invoke(std::move(f), std::move(args)...));
                }
            }
            catch (...)
            {
                promise->set_exception(std::current_exception());
            }
        };

        return std::make_pair(std::move(wrapper), std::move(future));
    }

    void setupWorker(uint16_t _idx);
    void startupWorker(uint16_t _idx);

    /**
     * @brief Tries to perform optimal assignment of a task to a worker.
     *
     * @return true if the task was successfully indirectly assigned to a worker or enqueued to the
     * global queue.
     * @return false if the thread pool is shutting down and cannot accept new tasks.
     */
    bool assignTaskToWorker(std::move_only_function<void()> _task) noexcept;

    /**
     * @brief Assigns a task directly to a specific worker by its ID.

     * @return true if the task was successfully directly assigned to the worker.
     * @return false if the thread pool is shutting down or the worker does not allow direct task
     * assignments.
     */
    bool assignTaskToWorkerById(uint16_t _idx, std::move_only_function<void()> _task) noexcept;

    /**
     * @brief Assigns a task directly to a specific worker by its debug name.
     *
     * @return true if the task was successfully directly assigned to the worker.
     * @return false if the thread pool is shutting down or the worker does not allow direct task
     * assignments.
     */
    bool assignTaskToWorkerByDebugName(const std::string& _debugName,
                                       std::move_only_function<void()> _task) noexcept;
};

} // namespace exec
} // namespace mosaic
