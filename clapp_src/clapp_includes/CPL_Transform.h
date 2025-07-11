/*
 *  Author:  Manoel McCadden
 *  Data:    05-15-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    CGL_Transform.h
 *
 *  Description:
 *    The interface file that hold transform data for a given entity
 */
#pragma once

#include "Clarity_Component.h"

// glm include for vec info
#include "../../external/glm/vec4.hpp"
#include "../../external/glm/vec3.hpp"
#include "../../external/glm/vec2.hpp"

#include "../../external/glm/mat4x4.hpp"

namespace ClaPP
{
class Transform : public Component
{
public:
  struct TransformData 
  {
    glm::vec3 worldPos;
    glm::vec3 scale;
    glm::vec3 rotation;

    glm::mat4 worldMatrix;
  };

  Transform(const glm::vec3 &worldPos = {0.f, 0.f, 0.f}
          , const glm::vec3 &scale = {1.f, 1.f, 1.f}
          , const glm::vec3 &rotation = {0.f, 0.f, 0.f})
  : transformData({worldPos, scale, rotation})
  {

  }
  ~Transform()
  {

  }

  TransformData &GetTransformData()
  {
    return transformData;
  }
private:
  TransformData transformData;
};
}
