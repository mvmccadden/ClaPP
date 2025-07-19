/*!
 *  \author  Manoel McCadden
 *  \date    06-06-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CIL_PlayerControllerSystem.cpp
 *
 *  \brief
 *    The implementation file for handling player input for entities
 *    and thier controllers
 */

// TODO: Create system that takes in a playercontroller component that
// handles input for the given entity
#include "clapp_includes/CIL_PlayerControllerSystem.h"

#include "clapp_includes/Clarity_Component.h"
#include "clapp_includes/Clarity_ECS.h"
#include "clapp_includes/CPL_Physics.h"
#include "clapp_includes/CIL_Controller.h"
#include "clapp_includes/CIL_Inputs.h"
#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_System.h"

using namespace ClaPP;
using namespace std;

PlayerControllerSystem::PlayerControllerSystem(const std::string &_sysName)
  : Clarity_System(_sysName)
{
  systemSignature.set(Component::C_CONTROLLER);
  systemSignature.set(Component::C_PHYSICS);
}

PlayerControllerSystem::~PlayerControllerSystem()
{

}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Initialize()
{
  return SYS_NO_ERR;
}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Load()
{
  return SYS_NO_ERR;
}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Update(float deltaTime)
{
  // Get the active ecs system
  ECS *ecs = ECS::GetActiveECS();

  // Get the input events from the input component
  KeyBindContainer *container = ecs->GetComponent<KeyBindContainer>(
    ecs->GetWorldID(), Component::C_INPUT);

  // Make sure the container exists
  if(!container)
  {
    ErrMessage("Failed to find input component in world", EC_INPUT);
    return SYS_FAILED_TO_UPDATE;
  }
  
  // Move all entites and their physics components into a physicsdata vector
  // to easy access and adjust them as events are sorted
  vector<Physics::PhysicsData*> vec_PhysicsData;

  // TODO: Update with controller having states of movement such as
  // this entity can be moved with editor movement but not game movement
  // so that only certain objects move within certain states... and 
  // possibly have it so that different systems handle these types of movements
  // to allow for more seperation of editor behavior and game behaviors
  
  // Setup default physicsData 
  Physics::PhysicsData physicsData;
  physicsData.appliedForce = {0.f, 0.f, 0.f};
  physicsData.rotationForce = {0.f, 0.f, 0.f};

  if(container->CheckEvent("MovePositiveX"))
  {
    physicsData.appliedForce += glm::vec3{0.03f, 0.f, 0.f};
  }
  if(container->CheckEvent("MoveNegativeX"))
  {
    physicsData.appliedForce += glm::vec3{-0.03f, 0.f, 0.f};
  }
  if(container->CheckEvent("MovePositiveY"))
  {
    physicsData.appliedForce += glm::vec3{0.f, 0.03f, 0.f};
  }
  if(container->CheckEvent("MoveNegativeY"))
  {
    physicsData.appliedForce += glm::vec3{0.f, -0.03f, 0.f};
  }
  if(container->CheckEvent("MovePositiveZ"))
  {
    physicsData.appliedForce += glm::vec3{0.f, 0.f, 0.03f};
  }
  if(container->CheckEvent("MoveNegativeZ"))
  {
    physicsData.appliedForce += glm::vec3{0.f, 0.f, -0.03f};
  }
  if(container->CheckEvent("RotatePositiveX"))
  {
    physicsData.rotationForce += glm::vec3{1.f, 0.f, 0.f};
  }
  if(container->CheckEvent("RotateNegativeX"))
  {
    physicsData.rotationForce += glm::vec3{-1.f, 0.f, 0.f};
  }
  if(container->CheckEvent("RotatePositiveY"))
  {
    physicsData.rotationForce += glm::vec3{0.f, 1.f, 0.f};
  }
  if(container->CheckEvent("RotateNegativeY"))
  {
    physicsData.rotationForce += glm::vec3{0.f, -1.f, 0.f};
  }
  if(container->CheckEvent("RotatePositiveZ"))
  {
    physicsData.rotationForce += glm::vec3{0.f, 0.f, 1.f};
  }
  if(container->CheckEvent("RotateNegativeZ"))
  {
    physicsData.rotationForce += glm::vec3{0.f, 0.f, -1.f};
  }

  // Update all entities with new force values
  for(const ENTITY_ID &entity : systemEntites)
  {
    // Get the required components
    Physics *physics = ecs->GetComponent<Physics>(
      entity, Component::C_PHYSICS);
    // NOTE: May not need the controller as the controller currently just acts
    // as an assurance that this entity can be controller... 
    // but this may change later so for right now we will retrieve it
    Controller *controller = ecs->GetComponent<Controller>(
      entity, Component::C_CONTROLLER);

    // Retrieve the data so we can modify it input updates
    Physics::PhysicsData &data = physics->GetPhysicsData();
    // As other systems maybe adjust these values we want to add to them
    // instead of overriding them!
    data.appliedForce += physicsData.appliedForce;
    data.rotationForce += physicsData.rotationForce;
  }

  return SYS_NO_ERR;
}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Render()
{
  return SYS_NO_ERR;
}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Unload()
{
  return SYS_NO_ERR;
}

PlayerControllerSystem::SYS_ERR PlayerControllerSystem::Terminate()
{
  return SYS_NO_ERR;
}
