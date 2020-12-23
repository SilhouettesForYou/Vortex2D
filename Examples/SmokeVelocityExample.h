//
//  SmokeVelocity.h
//  Vortex2D
//

#include "Vortex2D.h"

#include <box2D/box2D.h>

#include "Rigidbody.h"
#include "Runner.h"

#include <functional>
#include <memory>
#include <vector>

extern glm::vec4 green;
extern glm::vec4 gray;

class SmokeVelocityExample : public Runner
{
public:
  SmokeVelocityExample(const Vortex2D::Renderer::Device& device, const glm::ivec2& size, float dt)
      : source1(device, glm::vec2(5.0f))
      , source2(device, glm::vec2(5.0f))
      , force1(device, glm::vec2(5.0f))
      , force2(device, glm::vec2(5.0f))
      , density(device, size, vk::Format::eR8G8B8A8Unorm)
      , world(device, size, dt, Vortex2D::Fluid::Velocity::InterpolationMode::Linear)
      , clearObstacles({250.0f, 0.0f, 0.0f, 0.0f})
      , rWorld({0.0f, 0.0f})
      , solver(rWorld)
      , body(device,
             size,
             rWorld,
             b2_dynamicBody,
             Vortex2D::Fluid::RigidBody::Type::eWeak,
             {80.0f, 10.0f},
             2.0f)
      , solidPhi(device, body.mRigidbody.Phi())
  {
    world.FieldBind(density);
    world.AttachRigidBodySolver(solver);
    world.AddRigidbody(body.mRigidbody);

    source1.Position = force1.Position = {25.0f, 25.0f};
    source2.Position = force2.Position = {125.0f, 225.0f};

    source1.Colour = source2.Colour = gray;

    force1.Colour = {50.0f, 0.0f, 0.0f, 0.0f};
    force2.Colour = {-50.0f, -50.0f, 0.0f, 0.0f};

    solidPhi.Colour = green;
  }

  void Init(const Vortex2D::Renderer::Device& device,
            Vortex2D::Renderer::RenderTarget& renderTarget) override
  {
    // Draw liquid boundaries
    Vortex2D::Renderer::Rectangle area(device, glm::ivec2(250));
    area.Colour = glm::vec4(-1.0f);
    area.Position = glm::vec2(3.0f);

    Vortex2D::Renderer::Clear clearLiquid({1.0f, 0.0f, 0.0f, 0.0f});

    world.RecordLiquidPhi({clearLiquid, area}).Submit().Wait();

    // Draw sources and forces
    velocityRender = world.RecordVelocity({force1, force2}, Vortex2D::Fluid::VelocityOp::Set);
    densityRender = density.Record({source1, source2});

    // Draw rigid body
    body.mRigidbody.SetTransform({100.0f, 100.0f}, -45.0f);

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
  Vortex2D::Renderer::Ellipse source1, source2;
  Vortex2D::Renderer::Ellipse force1, force2;
  Vortex2D::Fluid::Density density;
  Vortex2D::Fluid::SmokeWorld world;
  Vortex2D::Renderer::Clear clearObstacles;
  Vortex2D::Renderer::RenderCommand velocityRender, densityRender, windowRender;

  b2World rWorld;

  Box2DSolver solver;
  RectangleRigidbody body;
  Vortex2D::Fluid::DistanceField solidPhi;
};
