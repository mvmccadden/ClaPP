/*!
 *  \author  Manoel McCadden
 *  \date    05-14-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Component.h
 *
 *  \brief
 *    An abstract class used to define what a component is
*/
#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <unordered_set>
#include <bitset>
#include <algorithm>
#include <typeinfo>

#include "Clarity_IO.h"

namespace ClaPP
{
// Definition for component signature and component limit
const size_t COMPONENT_COUNT = 64;
typedef std::bitset<COMPONENT_COUNT> COMPONENT_SIGNATURE;

// NOTE: The ECS system will hold pointers to the components and will be
// able to handle them but will not hold them directly... Instead the library
// and memorystorage device will hold them as needed respectivly. They will 
// each have their own creation function? based on specific needs. Ideally we 
// want them all to use the ctor but for mesh and other copyable/library based
// objects we may need to find an alternative
class Component
{
public:
  enum COMPONENTS
  {
    C_MESH = 0
    , C_TEXTURE
    , C_TRANSFORM
    , C_PHYSICS
    , C_INPUT
    , C_CONTROLLER

    , C_COUNT
    , C_INVALID
  };

  inline const static std::array<std::string, C_COUNT> componentMap =
  {
    "Mesh"               // C_MESH
    , "Texture"          // C_TEXTURE
    , "Transform"        // C_TRANSFORM
    , "Physics"          // C_PHYSICS
    , "KeyBindContainer" // C_INPUT
    , "Controller"       // C_CONTROLLER
  };

  Component();
  virtual ~Component();

  const std::string &ToString(const COMPONENTS &component);

  // TODO: Create a bi-direcitonal map to allow for easier 
  // look up from string to enum
  COMPONENTS ToEnum();

};
}
