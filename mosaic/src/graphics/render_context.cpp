#include "mosaic/graphics/render_context.hpp"

namespace mosaic
{
namespace graphics
{

void RenderContext::render()
{
    beginFrame();
    updateResources();
    drawScene();
    endFrame();
}

} // namespace graphics
} // namespace mosaic
