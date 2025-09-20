#pragma once

#include "mosaic/defines.hpp"

#include <pieces/core/result.hpp>

#include "mosaic/core/logger.hpp"

namespace mosaic
{
namespace core
{

/**
 * @brief Default logging sink that outputs logs to the console.
 */
class MOSAIC_API DefaultSink final : public core::Sink
{
   public:
    ~DefaultSink() override = default;

    pieces::RefResult<core::Sink, std::string> initialize() override;
    void shutdown() override;

    void trace(const std::string& _message) const override;
    void debug(const std::string& _message) const override;
    void info(const std::string& _message) const override;
    void warn(const std::string& _message) const override;
    void error(const std::string& _message) const override;
    void critical(const std::string& _message) const override;
};

} // namespace core
} // namespace mosaic
