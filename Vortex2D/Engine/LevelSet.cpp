//
//  LevelSet.cpp
//  Vortex2D
//

#include "LevelSet.h"

#include "Boundaries.h"
#include "../Renderer/CommandBuffer.h"

#include "../vortex2d_generated_spirv.h"

namespace Vortex2D
{
namespace Fluid
{
LevelSet::LevelSet(const Renderer::Device& device,
                   const glm::ivec2& size,
                   int reinitializeIterations)
    : Renderer::RenderTexture(device, size.x, size.y, vk::Format::eR32Sfloat)
    , mDevice(device)
    , mLevelSet0(device, size.x, size.y, vk::Format::eR32Sfloat)
    , mLevelSetBack(device, size.x, size.y, vk::Format::eR32Sfloat)
    , mSampler(Renderer::SamplerBuilder()
                   .AddressMode(vk::SamplerAddressMode::eClampToEdge)
                   .Create(device.Handle()))
    , mExtrapolate(device, size, SPIRV::Extrapolate_comp)
    , mRedistance(device, size, SPIRV::Redistance_comp)
    , mRedistanceFront(
          mRedistance.Bind({{*mSampler, mLevelSet0}, {*mSampler, *this}, mLevelSetBack}))
    , mRedistanceBack(
          mRedistance.Bind({{*mSampler, mLevelSet0}, {*mSampler, mLevelSetBack}, *this}))
    , mShrinkWrap(device, size, SPIRV::ShrinkWrap_comp)
    , mShrinkWrapBound(mShrinkWrap.Bind({{*mSampler, *this}, mLevelSetBack}))
    , mExtrapolateCmd(device, false)
    , mReinitialiseCmd(device, false)
    , mShrinkWrapCmd(device, false)
{
  mReinitialiseCmd.Record([&, reinitializeIterations](vk::CommandBuffer commandBuffer) {
    commandBuffer.debugMarkerBeginEXT({"Reinitialise", {{0.98f, 0.49f, 0.26f, 1.0f}}},
                                      mDevice.Loader());

    mLevelSet0.CopyFrom(commandBuffer, *this);

    for (int i = 0; i < reinitializeIterations / 2; i++)
    {
      mRedistanceFront.PushConstant(commandBuffer, 0.1f);
      mRedistanceFront.Record(commandBuffer);
      mLevelSetBack.Barrier(commandBuffer,
                            vk::ImageLayout::eGeneral,
                            vk::AccessFlagBits::eShaderWrite,
                            vk::ImageLayout::eGeneral,
                            vk::AccessFlagBits::eShaderRead);
      mRedistanceBack.PushConstant(commandBuffer, 0.1f);
      mRedistanceBack.Record(commandBuffer);
      Barrier(commandBuffer,
              vk::ImageLayout::eGeneral,
              vk::AccessFlagBits::eShaderWrite,
              vk::ImageLayout::eGeneral,
              vk::AccessFlagBits::eShaderRead);
    }

    commandBuffer.debugMarkerEndEXT(mDevice.Loader());
  });

  mShrinkWrapCmd.Record([&](vk::CommandBuffer commandBuffer) {
    commandBuffer.debugMarkerBeginEXT({"Shrink Wrap", {{0.36f, 0.71f, 0.38f, 1.0f}}},
                                      mDevice.Loader());
    mShrinkWrapBound.Record(commandBuffer);
    mLevelSetBack.Barrier(commandBuffer,
                          vk::ImageLayout::eGeneral,
                          vk::AccessFlagBits::eShaderWrite,
                          vk::ImageLayout::eGeneral,
                          vk::AccessFlagBits::eShaderRead);
    CopyFrom(commandBuffer, mLevelSetBack);
    commandBuffer.debugMarkerEndEXT(mDevice.Loader());
  });
}

LevelSet::LevelSet(LevelSet&& other)
    : Renderer::RenderTexture(std::move(other))
    , mDevice(other.mDevice)
    , mLevelSet0(std::move(other.mLevelSet0))
    , mLevelSetBack(std::move(other.mLevelSetBack))
    , mSampler(std::move(other.mSampler))
    , mExtrapolate(std::move(other.mExtrapolate))
    , mExtrapolateBound(std::move(other.mExtrapolateBound))
    , mRedistance(std::move(other.mRedistance))
    , mRedistanceFront(std::move(other.mRedistanceFront))
    , mRedistanceBack(std::move(other.mRedistanceBack))
    , mShrinkWrap(std::move(other.mShrinkWrap))
    , mShrinkWrapBound(std::move(other.mShrinkWrapBound))
    , mExtrapolateCmd(std::move(other.mExtrapolateCmd))
    , mReinitialiseCmd(std::move(other.mReinitialiseCmd))
    , mShrinkWrapCmd(std::move(other.mShrinkWrapCmd))
{
}

void LevelSet::ExtrapolateBind(Renderer::Texture& solidPhi)
{
  mExtrapolateBound = mExtrapolate.Bind({solidPhi, *this});
  mExtrapolateCmd.Record([&](vk::CommandBuffer commandBuffer) {
    commandBuffer.debugMarkerBeginEXT({"Extrapolate phi", {{0.53f, 0.09f, 0.16f, 1.0f}}},
                                      mDevice.Loader());
    mExtrapolateBound.Record(commandBuffer);
    Barrier(commandBuffer,
            vk::ImageLayout::eGeneral,
            vk::AccessFlagBits::eShaderWrite,
            vk::ImageLayout::eGeneral,
            vk::AccessFlagBits::eShaderRead);
    commandBuffer.debugMarkerEndEXT(mDevice.Loader());
  });
}

void LevelSet::Reinitialise()
{
  mReinitialiseCmd.Submit();
}

void LevelSet::ShrinkWrap()
{
  mShrinkWrapCmd.Submit();
}

void LevelSet::Extrapolate()
{
  mExtrapolateCmd.Submit();
}

}  // namespace Fluid
}  // namespace Vortex2D
