//
//  Runner.h
//  Vortex2D
//

#ifndef Examples_Runner_h
#define Examples_Runner_h

#include "Renderer/Device.h"
#include "Renderer/RenderTarget.h"

class Runner
{
public:
  virtual ~Runner() {}
  virtual void Init(const Vortex2D::Renderer::Device& device,
                    Vortex2D::Renderer::RenderTarget& renderTarget) = 0;
  virtual void Step() = 0;
};

#endif
