/*
 *  \author  Manoel McCadden
 *  \date    05-15-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_Physics.h
 *
 *  \brief
 *    The interface file that hold physics data for a given entity
 */
#pragma once

#include <queue>

#include "Clarity_Component.h"

// glm include for vec info
#include "../../external/glm/vec4.hpp"
#include "../../external/glm/vec3.hpp"
#include "../../external/glm/vec2.hpp"

namespace ClaPP
{
class Physics : public Component
{
public:
  struct PhysicsData
  {
    // Base physics properties
    glm::vec3 acceleration = {0.f, 0.f, 0.f};
    glm::vec3 veclotiy = {0.f, 0.f, 0.f};
    // Applied physics properties
    glm::vec3 appliedForce = {0.f, 0.f, 0.f};
    glm::vec3 rotationForce = {0.f, 0.f, 0.f};
    // Character Details
    float speed = 0.05f;
    // Collision properties
    
    // Simple bools
    bool gravityOn = true;
    bool collisionOn = true;
  };

  Physics()
  : physicsData()
  {

  }
  Physics(const PhysicsData &_physicsData)
  : physicsData(_physicsData)
  {

  }
  ~Physics()
  {

  }

  PhysicsData &GetPhysicsData()
  {
    return physicsData;
  }
private:
  PhysicsData physicsData;
};
}
