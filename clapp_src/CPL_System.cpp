/*!
 *  \author  Manoel McCadden
 *  \date    05-15-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CPL_System.cpp
 *
 *  \brief
 *    The implementation file for interacting with the physics system
 */

#include "clapp_includes/Clarity_Component.h"
#include "clapp_includes/pch.h"

#include "clapp_includes/CPL_System.h"

#include <cstdlib>

#include "clapp_includes/CPL_Transform.h"
#include "clapp_includes/CPL_Physics.h"
#include "clapp_includes/CIL_Inputs.h"

#include "clapp_includes/Clarity_ECS.h"
#include "clapp_includes/Clarity_IO.h"

#include "clapp_includes/g_pch.h"

using namespace ClaPP;
using namespace std;

// NOTE: Temp constant drag;
static const glm::vec3 DRAG = {3.f, 3.f, 3.f};

CPL_System::CPL_System(const std::string &_sysName)
: Clarity_System(_sysName), gravityVec(defaultGravityVec)
{
  systemSignature.set(Component::C_TRANSFORM);
  systemSignature.set(Component::C_PHYSICS);
}

CPL_System::~CPL_System()
{

}

CPL_System::SYS_ERR CPL_System::Initialize()
{
  return SYS_NO_ERR;
}

CPL_System::SYS_ERR CPL_System::Load()
{
  return SYS_NO_ERR;
}

CPL_System::SYS_ERR CPL_System::Update(float deltaTIme)
{
  // TODO: Make it so that the system reflects the changes in force
  // done/inputs made to the player
  ECS *ecs = GetECSPtr();
  glm::mat4 identity = glm::identity<glm::mat4>();

  for(const ENTITY_ID &entity : systemEntites)
  {
    // Retrieve the transform and physics components to modify them
    Transform *transform = 
      ecs->GetComponent<Transform>(entity, Component::C_TRANSFORM);
    Physics *physics = 
      ecs->GetComponent<Physics>(entity, Component::C_PHYSICS);

    // Skip if for some reason they are nullptr and report an error
    if(transform == nullptr || physics == nullptr)
    {
      ErrMessage("Entity ID: " + std::to_string(entity)
                 + " has an invalid transform or physics component"
                 , CPLSYSERR);
      continue;
    }

    // Get the proper data types
    Physics::PhysicsData &physicsData = physics->GetPhysicsData();
    Transform::TransformData &transformData = transform->GetTransformData();

    transformData.worldPos += physicsData.appliedForce;
    transformData.rotation += physicsData.rotationForce;
    
    glm::mat4 translate = glm::translate(identity, transformData.worldPos);
    glm::mat4 scale = glm::scale(identity, transformData.scale);
    // Rotate on all axis's
    // NOTE: Added some temp rotations on all axis
    glm::mat4 rotateX = glm::rotate(identity
                                    , glm::radians(
                                    transformData.rotation.x)
                                    , {1.f, 0.f, 0.f});
    glm::mat4 rotateY = glm::rotate(identity
                                    , glm::radians(
                                    transformData.rotation.y)
                                    , {0.f, 1.f, 0.f});
    glm::mat4 rotateZ = glm::rotate(identity
                                    , glm::radians(
                                    transformData.rotation.z)
                                    , {0.f, 0.f, 1.f});

    // Create a correspongding world-based matrix
    transformData.worldMatrix = translate * rotateZ * rotateY * rotateX * scale;

    // Reset applied force
    physicsData.appliedForce = {0.f, 0.f, 0.f};
    physicsData.rotationForce = {0.f, 0.f, 0.f};
  }

  return SYS_NO_ERR;
}

CPL_System::SYS_ERR CPL_System::Render()
{
  return SYS_NO_ERR;
}

CPL_System::SYS_ERR CPL_System::Unload()
{
  return SYS_NO_ERR;
}

CPL_System::SYS_ERR CPL_System::Terminate()
{
  return SYS_NO_ERR;
}


