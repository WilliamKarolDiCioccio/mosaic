#pragma once

#include "window.hpp"

namespace mosaic::graphics
{

class RendererAPI
{
   public:
    virtual ~RendererAPI() = default;

    virtual void initialize(const Window& _window) = 0;
    virtual void shutdown() = 0;
    virtual void clearScreen() = 0;
};

} // namespace mosaic::graphics
