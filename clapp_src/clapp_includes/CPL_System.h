/*
 *  \author  Manoel McCadden
 *  \date    05-15-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CPL_System.h
 *
 *  \brief
 *    The interface file for interacting with the physics system
 */
#pragma once

#include "Clarity_System.h"


// glm include for vec info
#include "../../external/glm/vec4.hpp"
#include "../../external/glm/vec3.hpp"
#include "../../external/glm/vec2.hpp"

#include "../../external/glm/mat4x4.hpp"

namespace ClaPP
{
class CPL_System : public Clarity_System
{
public:
  CPL_System(const std::string &_sysName);
  ~CPL_System();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  void SetGravityVec(const glm::vec3 &_gravityVec);
  const glm::vec3 &GetGravityVec();
  void ResetGravityVec();

private:
  inline const static glm::vec3 defaultGravityVec = {0.f, -9.81f, 0.f};
  glm::vec3 gravityVec;
  
  CPL_System(const CPL_System &other) = delete;
  CPL_System &operator=(const CPL_System &other) = delete;
};
}
