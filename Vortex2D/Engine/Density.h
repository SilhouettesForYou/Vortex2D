//
//  Density.h
//  Vortex2D
//

#ifndef Vortex2D_Density_h
#define Vortex2D_Density_h

#include "../Renderer/RenderTexture.h"
#include "../Renderer/Sprite.h"

namespace Vortex2D
{
namespace Fluid
{
/**
 * @brief Density field, used to represent smoke swirling.
 */
class Density : public Renderer::RenderTexture, public Renderer::Sprite
{
public:
  VORTEX2D_API Density(const Renderer::Device& device, const glm::ivec2& size, vk::Format format);

  friend class Advection;

private:
  Renderer::Texture mFieldBack;
};

}  // namespace Fluid
}  // namespace Vortex2D

#endif
