//
//  IncompletePoisson.h
//  Vortex2D
//

#ifndef Vortex2D_IncompletePoisson_h
#define Vortex2D_IncompletePoisson_h

#include "Preconditioner.h"
#include "../../Renderer/CommandBuffer.h"
#include "../../Renderer/Work.h"

namespace Vortex2D
{
namespace Fluid
{
/**
 * @brief Incomplete poisson preconditioner. Slightly better than a simple
 * diagonal preconditioner.
 */
class IncompletePoisson : public Preconditioner
{
public:
  VORTEX2D_API IncompletePoisson(const Renderer::Device& device, const glm::ivec2& size);
  VORTEX2D_API ~IncompletePoisson() override;

  VORTEX2D_API void Bind(Renderer::GenericBuffer& d,
                         Renderer::GenericBuffer& l,
                         Renderer::GenericBuffer& b,
                         Renderer::GenericBuffer& pressure) override;

  void Record(vk::CommandBuffer) override;

private:
  Renderer::Work mIncompletePoisson;
  Renderer::Work::Bound mIncompletePoissonBound;
};

}  // namespace Fluid
}  // namespace Vortex2D

#endif
