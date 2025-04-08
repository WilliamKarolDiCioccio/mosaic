#pragma once

#include <type_traits>

namespace mosaic
{
namespace utils
{

/**
 * @brief A template class for handling bit flags using C++ enum classes.
 *
 * The EnumFlags class allows you to work with C++ enum classes as if they were bit flags,
 * enabling bitwise operations such as AND, OR, XOR, and assignment on enum values.
 *
 * @tparam E The enum class type to be used as bit flags.
 */
template <typename E>
class EnumFlags final
{
    static_assert(std::is_enum_v<E>, "Type is not an enum!");

    using UnderlyingType = std::underlying_type_t<E>;

    static_assert(std::is_integral_v<UnderlyingType>, "Type is not integral!");
    static_assert(std::is_unsigned_v<UnderlyingType>, "Type is not unsigned!");

   private:
    /**< The underlying integer value representing the combined bit flags. */
    UnderlyingType m_flags;

   public:
    /**
     * @brief Default constructor initializes the flags to zero.
     */
    EnumFlags() : m_flags(0) {};
    EnumFlags(E _enum) : m_flags(UnderlyingType(_enum)) {};

   public:
    /**
     * @brief Assignment operator for setting the flags using an enum value.
     *
     * @param _enum The enum value to set the flags to.
     */
    constexpr void operator=(E _enum) noexcept { m_flags = (UnderlyingType)_enum; }

    /**
     * @brief Bitwise AND assignment operator to perform AND operation with an enum value.
     *
     * @param _enum The enum value to AND with the existing flags.
     */
    constexpr void operator&=(E _enum) noexcept { m_flags &= (UnderlyingType)_enum; }

    /**
     * @brief Bitwise OR assignment operator to perform OR operation with an enum value.
     *
     * @param _enum The enum value to OR with the existing flags.
     */
    constexpr void operator|=(E _enum) noexcept { m_flags |= (UnderlyingType)_enum; }

    /**
     * @brief Bitwise XOR assignment operator to perform XOR operation with an enum value.
     *
     * @param _enum The enum value to XOR with the existing flags.
     */
    constexpr void operator^=(E _enum) noexcept { m_flags ^= (UnderlyingType)_enum; }

    /**
     * @brief Bitwise AND operator to check if a specific flag is set.
     *
     * @param _enum The enum value to check.
     * @return bool True if the specified flag is set; otherwise, false.
     */
    constexpr bool operator&(E _enum) const noexcept { return m_flags & (UnderlyingType)_enum; }

    /**
     * @brief Bitwise OR operator to check if a specific flag is set.
     *
     * @param _enum The enum value to check.
     * @return bool True if the specified flag is set; otherwise, false.
     */
    constexpr bool operator|(E _enum) const noexcept { return m_flags | (UnderlyingType)_enum; }

    /**
     * @brief Bitwise XOR operator to check if a specific flag is set.
     *
     * @param _enum The enum value to check.
     * @return bool True if the specified flag is set; otherwise, false.
     */
    constexpr bool operator^(E _enum) const noexcept { return m_flags ^ (UnderlyingType)_enum; }

    /**
     * @brief Comparison operator to check if the flags are equal to a specific enum value.
     *
     * @param _enum The enum value to compare with.
     * @return bool True if the flags are equal to the specified enum value; otherwise, false.
     */
    constexpr bool operator==(E _enum) const noexcept { return m_flags == (UnderlyingType)_enum; }

    /**
     * @brief Comparison operator to check if the flags are not equal to a specific enum value.
     *
     * @param _enum The enum value to compare with.
     * @return bool True if the flags are not equal to the specified enum value; otherwise, false.
     */
    constexpr bool operator!=(E _enum) const noexcept { return m_flags != (UnderlyingType)_enum; }

   public:
    /**
     * @brief Get the combined bit flags as an underlying integral type.
     *
     * @return UnderlyingType The combined bit flags as an integral type.
     */
    UnderlyingType getFlags() const noexcept { return (UnderlyingType)m_flags; }
};

} // namespace utils
} // namespace mosaic
