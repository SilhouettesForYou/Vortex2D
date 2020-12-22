//
//  Jacobi.h
//  Vortex2D
//

#ifndef Vortex2D_Jacobi_h
#define Vortex2D_Jacobi_h

#include "LinearSolver.h"
#include "Preconditioner.h"
#include "Reduce.h"
#include "../../Renderer/CommandBuffer.h"
#include "../../Renderer/Pipeline.h"
#include "../../Renderer/Work.h"

namespace Vortex2D
{
namespace Fluid
{
/**
 * @brief An iterative jacobi linear solver.
 */
class Jacobi : public Preconditioner
{
public:
  Jacobi(const Renderer::Device& device, const glm::ivec2& size);

  void Bind(Renderer::GenericBuffer& d,
            Renderer::GenericBuffer& l,
            Renderer::GenericBuffer& b,
            Renderer::GenericBuffer& pressure) override;

  void Record(vk::CommandBuffer commandBuffer) override;

  void Record(vk::CommandBuffer commandBuffer, int iterations);

  /**
   * @brief Set the w factor of the GS iterations : x_new = w * x_new + (1-w) *
   * x_old
   * @param w
   */
  void SetW(float w);

  /**
   * @brief set number of iterations to be used when GS is a preconditioner
   * @param iterations
   */
  void SetPreconditionerIterations(int iterations);

private:
  float mW;
  int mPreconditionerIterations;

  Renderer::GenericBuffer* mPressure;
  Renderer::Buffer<float> mBackPressure;

  Renderer::Work mJacobi;
  Renderer::Work::Bound mJacobiFrontBound;
  Renderer::Work::Bound mJacobiBackBound;
};

}  // namespace Fluid
}  // namespace Vortex2D

#endif
