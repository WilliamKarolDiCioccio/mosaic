#pragma once

#include "../context/vulkan_device.hpp"

namespace mosaic
{
namespace graphics
{
namespace vulkan
{

using CommandPool = VkCommandPool;

void createCommandPool(CommandPool& _commandPool, const Device& _device, const Surface& _surface);

void destroyCommandPool(CommandPool& _commandPool, const Device& _device);

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
