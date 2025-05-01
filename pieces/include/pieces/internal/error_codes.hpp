#pragma once

namespace pieces
{

enum class ErrorCode
{
    // Resource errors
    out_of_memory,
    resource_exhausted,
    resource_unavailable,

    // Range/boundary errors
    out_of_range,
    index_out_of_bounds,
    buffer_overflow,

    // Argument/input errors
    invalid_argument,
    null_pointer,
    type_mismatch,
    wrong_size,
    value_too_large,

    // State errors
    invalid_operation,
    operation_not_permitted,
    already_initialized,
    not_initialized,
    already_exists,
    does_not_exist,
    container_empty,
    container_full,

    // Threading/concurrency errors
    mutex_error,
    deadlock_detected,
    thread_creation_failed,
    concurrent_modification,

    // I/O and external errors
    io_error,
    file_not_found,
    permission_denied,
    read_error,
    write_error,

    // Format errors
    parse_error,
    format_error,
    serialization_error,

    // System errors
    system_error,
    not_implemented,
    not_supported,
    timeout,

    // Container-specific errors (e.g., for maps)
    key_not_found,
    duplicate_key,

    // General errors
    unknown_error,
    internal_error
};

} // namespace pieces
