//
//  LevelSet.h
//  Vortex2D
//

#ifndef LevelSet_h
#define LevelSet_h

#include "../Renderer/CommandBuffer.h"
#include "../Renderer/RenderTexture.h"
#include "../Renderer/Work.h"

namespace Vortex2D
{
namespace Fluid
{
/**
 * @brief A signed distance field, which can be re-initialized. In other words,
 * a level set.
 */
class LevelSet : public Renderer::RenderTexture
{
public:
  VORTEX2D_API LevelSet(const Renderer::Device& device,
                        const glm::ivec2& size,
                        int reinitializeIterations = 50);

  VORTEX2D_API LevelSet(LevelSet&& other);

  /**
   * @brief Reinitialise the level set, i.e. ensure it is a correct signed
   * distance field.
   */
  VORTEX2D_API void Reinitialise();

  /**
   * @brief Shrink wrap wholes.
   */
  VORTEX2D_API void ShrinkWrap();

  /**
   * @brief Bind a solid level set, which will be used to extrapolate into this
   * level set
   * @param solidPhi
   */
  VORTEX2D_API void ExtrapolateBind(Renderer::Texture& solidPhi);

  /**
   * @brief Extrapolate this level set into the solid level set it was attached
   * to. This only performs a single cell extrapolation.
   */
  VORTEX2D_API void Extrapolate();

private:
  const Renderer::Device& mDevice;
  Renderer::Texture mLevelSet0;
  Renderer::Texture mLevelSetBack;

  vk::UniqueSampler mSampler;

  Renderer::Work mExtrapolate;
  Renderer::Work::Bound mExtrapolateBound;
  Renderer::Work mRedistance;
  Renderer::Work::Bound mRedistanceFront;
  Renderer::Work::Bound mRedistanceBack;
  Renderer::Work mShrinkWrap;
  Renderer::Work::Bound mShrinkWrapBound;

  Renderer::CommandBuffer mExtrapolateCmd;
  Renderer::CommandBuffer mReinitialiseCmd;
  Renderer::CommandBuffer mShrinkWrapCmd;
};

}  // namespace Fluid
}  // namespace Vortex2D

#endif
