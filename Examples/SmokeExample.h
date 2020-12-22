//
//  Smoke.h
//  Vortex2D
//

#include "../Vortex2D/Vortex2D.h"

#include "Runner.h"

#include <functional>
#include <memory>
#include <vector>

extern glm::vec4 green;
extern glm::vec4 gray;

class SmokeExample : public Runner
{
public:
  SmokeExample(const Vortex2D::Renderer::Device& device, const glm::ivec2& size, float dt)
      : source1(device, glm::vec2(20.0f))
      , source2(device, glm::vec2(20.0f))
      , force1(device, glm::vec2(20.0f))
      , force2(device, glm::vec2(20.0f))
      , density(device, size, vk::Format::eR8G8B8A8Unorm)
      , world(device, size, dt, Vortex2D::Fluid::Velocity::InterpolationMode::Linear)
      , solidPhi(world.SolidDistanceField())
  {
    world.FieldBind(density);

    source1.Position = force1.Position = {75.0f, 25.0f};
    source2.Position = force2.Position = {175.0f, 225.0f};

    source1.Anchor = source2.Anchor = glm::vec2(10.0);
    force1.Anchor = force2.Anchor = glm::vec2(10.0);

    source1.Colour = source2.Colour = gray;

    force1.Colour = {0.0f, 30.0f, 0.0f, 0.0f};
    force2.Colour = {0.0f, -30.0f, 0.0f, 0.0f};

    solidPhi.Colour = green;
  }

  void Init(const Vortex2D::Renderer::Device& device,
            Vortex2D::Renderer::RenderTarget& renderTarget) override
  {
    // Draw liquid boundaries
    Vortex2D::Renderer::Rectangle area(device, glm::ivec2(256) - glm::ivec2(4));
    area.Colour = glm::vec4(-1);
    area.Position = glm::vec2(2.0f);

    Vortex2D::Renderer::Clear clearLiquid({1.0f, 0.0f, 0.0f, 0.0f});

    world.RecordLiquidPhi({clearLiquid, area}).Submit().Wait();

    // Draw solid boundaries
    Vortex2D::Fluid::Circle obstacle1(device, 15.0f);
    Vortex2D::Fluid::Circle obstacle2(device, 15.0f);

    obstacle1.Position = {75.0f, 100.0f};
    obstacle2.Position = {175.0f, 125.0f};

    world.RecordStaticSolidPhi({Vortex2D::Fluid::BoundariesClear, obstacle1, obstacle2})
        .Submit()
        .Wait();

    // Draw sources and forces
    velocityRender = world.RecordVelocity({force1, force2}, Vortex2D::Fluid::VelocityOp::Set);
    densityRender = density.Record({source1, source2});

    Vortex2D::Renderer::ColorBlendState blendState;
    blendState.ColorBlend.setBlendEnable(true)
        .setAlphaBlendOp(vk::BlendOp::eAdd)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero);

    windowRender = renderTarget.Record({density, solidPhi}, blendState);
  }

  void Step() override
  {
    velocityRender.Submit();
    densityRender.Submit();

    auto params = Vortex2D::Fluid::FixedParams(12);
    world.Step(params);

    windowRender.Submit();
  }

private:
  Vortex2D::Renderer::Rectangle source1, source2;
  Vortex2D::Renderer::Rectangle force1, force2;
  Vortex2D::Fluid::Density density;
  Vortex2D::Fluid::SmokeWorld world;
  Vortex2D::Fluid::DistanceField solidPhi;
  Vortex2D::Renderer::RenderCommand velocityRender, densityRender, windowRender;
};
