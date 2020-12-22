//
//  Velocity.h
//  Vortex2D
//

#ifndef Vortex2d_Velocity_h
#define Vortex2d_Velocity_h

#include "../Renderer/CommandBuffer.h"
#include "../Renderer/Device.h"
#include "../Renderer/RenderTexture.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Work.h"

namespace Vortex2D
{
namespace Fluid
{
/**
 * @brief The Velocity field. Can be used to calculate a difference between
 * different states. Contains three fields: intput and output, used for
 * ping-pong algorithms, and d, the difference between two velocity fields.
 */
class Velocity : public Renderer::RenderTexture
{
public:
  /**
   * @brief Velocity interpolation when querying in the shader with non-integer locations.
   */
  enum class InterpolationMode : int
  {
    Linear = 0,
    Cubic = 1,
  };

  VORTEX2D_API Velocity(const Renderer::Device& device, const glm::ivec2& size);

  /**
   * @brief An output texture used for algorithms that used the velocity as
   * input and need to create a new velocity field
   * @return
   */
  VORTEX2D_API Renderer::Texture& Output();

  /**
   * @brief A difference velocity field, calculated with the difference between
   * this velocity field, and the output velocity field
   * @return
   */
  VORTEX2D_API Renderer::Texture& D();

  /**
   * @brief Copy the output field to the main field
   * @param commandBuffer
   */
  VORTEX2D_API void CopyBack(vk::CommandBuffer commandBuffer);

  /**
   * @brief Clear the velocity field
   * @param commandBuffer
   */
  VORTEX2D_API void Clear(vk::CommandBuffer commandBuffer);

  /**
   * @brief Copy to the difference field.
   */
  VORTEX2D_API void SaveCopy();

  /**
   * @brief Calculate the difference between the difference field and this
   * velocity field, store it in the diference field.
   */
  VORTEX2D_API void VelocityDiff();

private:
  const Renderer::Device& mDevice;
  Renderer::Texture mOutputVelocity;
  Renderer::Texture mDVelocity;

  Renderer::Work mVelocityDiff;
  Renderer::Work::Bound mVelocityDiffBound;

  Renderer::CommandBuffer mSaveCopyCmd;
  Renderer::CommandBuffer mVelocityDiffCmd;
};

}  // namespace Fluid
}  // namespace Vortex2D

#endif
