#pragma once

#include <string>

namespace mosaic
{
namespace graphics
{

enum class BufferUsage
{
    Vertex,
    Index,
    Uniform,
    Storage
};

struct BufferDescription
{
    size_t size;
    BufferUsage usage;
    bool cpuWritable;
    std::string debugName;
};

} // namespace graphics
} // namespace mosaic
