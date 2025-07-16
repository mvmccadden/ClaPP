/*
 *  Author:  Manoel McCadden
 *  Data:    07-10-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    Clarity_ECS.cpp
 *
 *  Description:
 *    An ECS manager that handles the creation, deletion, and managment of
 *    entites, components, and systems
*/

// PCH and H files
#include "clapp_includes/pch.h"
#include "clapp_includes/Clarity_ECS.h"

// ECS Pieces
#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_System.h"
#include "clapp_includes/Clarity_Entity.h"
#include "clapp_includes/Clarity_Component.h"

// NOTE: Temp input inclusion for world singleton components
#include "clapp_includes/CIL_Inputs.h"

using namespace std;
using namespace ClaPP;

// TODO: Adjust all component-related ECS methods to take component type
// instead of taking an enum

//=================//
//= CTOR and DTOR =//
//=================//

ECS::ECS() : Clarity_System("ECS"), worldID(NEW_ENTITY_ID++)
{
  // We increment the new entity id by 1 as we will use the first new entity
  // id by default (which should be 0u)
  activeECS = this;
  // Assign world components 
  AddWorldSingletonComponents();
}

ECS::~ECS()
{
  // NOTE: System cleanup is done in terminate so it MUST be called before
  // the ECS is destroyed
  ClearEntities();
}

//==================//
//= Public Methods =//
//==================//

//==================//
//= System Methods =//
//==================//

ECS::SYS_ERR ECS::Initialize()
{
  activeECS = this;

  SYS_ERR err;
  for(Clarity_System *&system : systems)
  {
    if((err = system->Initialize()) != SYS_NO_ERR)
    {
      return err;
    }
  }

  return SYS_NO_ERR;
}


ECS::SYS_ERR ECS::Load()
{
  activeECS = this;

  SYS_ERR err;
  for(Clarity_System *&system : systems)
  {
    if((err = system->Load()) != SYS_NO_ERR)
    {
      return err;
    }
  }

  return SYS_NO_ERR;
}

ECS::SYS_ERR ECS::Update(float deltaTime)
{
  activeECS = this;

  SYS_ERR err;
  for(Clarity_System *&system : systems)
  {
    if((err = system->Update(deltaTime)) != SYS_NO_ERR)
    {
      return err;
    }
  }

  return SYS_NO_ERR;
}

ECS::SYS_ERR ECS::Render()
{
  activeECS = this;

  SYS_ERR err;
  for(Clarity_System *&system : systems)
  {
    if((err = system->Render()) != SYS_NO_ERR)
    {
      return err;
    }
  }

  return SYS_NO_ERR;
}

ECS::SYS_ERR ECS::Unload()
{
  activeECS = this;

  SYS_ERR err;
  // We do this in reverse as we want to make sure that systems that are
  // possible dependant on other systems are unloaded first
  for(std::vector<Clarity_System *>::reverse_iterator it = systems.rbegin()
  ; it != systems.rend(); ++it)
  {
    if((err = (*it)->Unload()) != SYS_NO_ERR)
    {
      return err;
    }
  }

  return SYS_NO_ERR;
}

ECS::SYS_ERR ECS::Terminate()
{
  activeECS = this;

  SYS_ERR err;
  // We do this in reverse as we want to make sure that systems that are
  // possible dependant on other systems are terminated first
  for(std::vector<Clarity_System *>::reverse_iterator it = systems.rbegin()
  ; it != systems.rend(); ++it)
  {
    if((err = (*it)->Terminate()) != SYS_NO_ERR)
    {
      return err;
    }
    delete *it;
  }

  return SYS_NO_ERR;
}

//====================//
//= Entity Functions =//
//====================//

/*!
  *  Creates a new entity id that can be used to identity an entity.
  *  Data in components can be added to the entity allowing for different
  *  systems to interact with it.
  *
  *  \returns
  *    The new ENTITY_ID that was just created
*/
ENTITY_ID ECS::CreateEntity()
{
  ENTITY_ID newID = 0u;

  if(deletedEntities.size() > 0)
  {
    newID = deletedEntities.front();
    deletedEntities.pop();
  }
  else
  {
    newID = NEW_ENTITY_ID++;
  }

  entities.insert(newID);
  return newID;
}

/*!
 *  Remove an entity and it's components from all systems, maps, and sets
 *  within the ECS
 *
 *  \param id
 *    The ENTITY_ID being removed from the ECS and all its dependants
 */
void ECS::DeleteEntity(const ENTITY_ID &id)
{
  if(entities.find(id) != entities.end())
  {
    entities.erase(id);
    deletedEntities.push(id);
    RemoveEntityFromAllSystems(id);
    RemoveEntityComponents(id);
  }
  else
  {
    ErrMessage("Failed to find given entity with ID: "
                + std::to_string(id), ECSERR);
  }
}

//=======================//
//= Component Functions =//
//=======================//

/*!
 *  Adds a component to a given entity by storing the component within
 *  the component map based on it's type and parent entity
 *
 *  \param id
 *    The entity that is having a component added to it
 *  \param componentType
 *    The type of the component being added
 *  \param component
 *    A raw pointer to the component being added
 */
void ECS::AddComponent(const ENTITY_ID &id
                       , Component *component)
{
  Component::COMPONENTS componentType = component->ToEnum();

  // Attempt to emplace a new component into the component tracker
  auto emplaced = components[
    static_cast<size_t>(componentType)].try_emplace(id, component);

  if(!emplaced.second)
  {
    ErrMessage("Failed to add component to entity with id: " 
                + std::to_string(id), ECSERR);
    return;
  }

  // Identity if a signature already exists
  std::unordered_map<ENTITY_ID, COMPONENT_SIGNATURE>::iterator signature 
    = entitySignatures.find(id);

  if(signature == entitySignatures.end())
  {
    // If not then create a new signature
    auto emplaced = entitySignatures.try_emplace(id);
    if(!emplaced.second)
    {
      ErrMessage("Failed to allocate a new component signature for"
                  " entity id: " + std::to_string(id), ECSERR);
      return;
    }

    // Set the signature to the newly emplaced signature
    signature = emplaced.first;
  }

  // Set the signatures corresponding bit to true
  signature->second.set(static_cast<size_t>(componentType));

  // Update the systems
  UpdateEntityInSystems(id);
}

/*!
 *  Removes a component from a given entity
 *
 *  \param id
 *    The entity being adjusted
 *  \param componentType
 *    The component being removed from the given entity
 *
 *    TODO: ADJUST COMPONENT ENUM TO TYPE
 */
void ECS::RemoveComponent(const ENTITY_ID &id
                          , const Component::COMPONENTS &componentType)
{
  // Get the signature from the given id
  std::unordered_map<ENTITY_ID, COMPONENT_SIGNATURE>::iterator signature
    = entitySignatures.find(id);

  if(signature == entitySignatures.end())
  {
    ErrMessage("Signature not found for given entity: " 
               + std::to_string(id), ECSERR);
    return;
  }

  size_t componentID = static_cast<size_t>(componentType);

  // Sets the corresponding components bit to false
  signature->second.reset(componentID);

  // Attempt to locate the component being deleted
  std::unordered_map<ENTITY_ID, Component *>::iterator component
    = components[static_cast<size_t>(componentType)].find(id);

  if(component == components[componentID].end())
  {
    ErrMessage("Failed to find component matching type and signature for "
               "given id: " + std::to_string(id), ECSERR);
    return;
  }

  // Deallocate the component then erase it from the component map
  // TODO: Rework with new allocation method once implemented
  delete component->second;
  components[componentID].erase(component);

  // Update systems to mimic entity
  UpdateEntityInSystems(id);
}

//====================//
//= System Functions =//
//====================//

/*!
 *  Adds a system to the ECS system list. Goes through active entity set
 *  and adds any entities with matching signatures to the system to the
 *  systems entity set
 *
 *  \param system
 *    The system being added to the ECS
 *
 */
void ECS::AddSystem(Clarity_System *system)
{
  // TODO: Remove the ecs ptr that each system has to enforce using
  // only the active ECS
  system->ecsPtr = this;
  systems.push_back(system);

  const COMPONENT_SIGNATURE &systemSignature = system->systemSignature;

  for(const ENTITY_ID &entity : entities) 
  {
    // Compare the signatures and add to the system if that are compatible
    if(entitySignatures[entity] == systemSignature)
    {
      system->AddEntityInSystem(entity);
    }
  }
}

//======================//
//= Singleton Entities =//
//======================//

/*!
 *  Each ECS instance has a set of global variables such as the world which
 *  holds and handles singleton component data such as inputs
 *
 *  \returns
 *    A const reference to the world entity which holds input data
 */
const ENTITY_ID &ECS::GetWorldID()
{
  return worldID;
}

//==========================//
//= Global Access Function =//
//==========================//

/*!
 *  There is always one active ECS (the last one to be called).
 *  Ideally there should only be one instance of any ECS but in the case
 *  that the user wants multiple ECS systems then this helps us minimze 
 *  ECS overlap.
 *
 * TODO: Ensure that only one ECS can be used per thread butttt systems witnin
 * the ecs can be in multiple threads
 *
 *  \returns 
 *    The currently active ECS instance
 */
ECS *ECS::GetActiveECS()
{
  return activeECS;
}

//===================//
//= Private Methods =//
//===================//

/*!
 *  Updates all systems to either add, remove, or verify an entites existance
 *  based on the entity's and system's signature
 *
 *  \param id
 *    The entity which is being updated in all the systems
 */
void ECS::UpdateEntityInSystems(const ENTITY_ID &id)
{
  // When adding entities, double check that each system can use it
  // i.e. the signatures are compatible with one another
  COMPONENT_SIGNATURE entitySignature = entitySignatures[id];

  for(Clarity_System *&system : systems)
  {
    const COMPONENT_SIGNATURE &systemSignature = system->systemSignature;

    // Verify that the system and entity have the same bits flipped.
    // If they do then we can add it to the system
    if((systemSignature & entitySignature) == systemSignature)
    {
      // As this already checks if the entity is already added we will
      // let this check for us to reduce redundancy
      system->AddEntityInSystem(id);
    }
    // If they don't we must remove the entity if it exists... as remove entity
    // also checks for us we will just remove it regardless of if it does or
    // doesn't exists
    else
    {
      system->RemoveEntityInSystem(id);
    }
  }
}

/*!
 *  Remove a given entity from all systems within the ECS
 *
 *  \param id
 *    The entity which will be removed from all systems
 */
void ECS::RemoveEntityFromAllSystems(const ENTITY_ID &id)
{
  // Iterate through the system vector deleting all instances of the entity
  for(Clarity_System *&system : systems)
  {
    RemoveEntityInSystem(id);
  }
}

/*!
 *  Handles removing all components of a given entity from the component map
 *
 *  \param id
 *    The id of the entity which will have it's components deleted
 */
void ECS::RemoveEntityComponents(const ENTITY_ID &id)
{
  // TODO: Look into changing this so it uses the entities signature to find
  // and delete components
  
  // Get each component map from the components array and find the
  // desired entities component if it exists.
  for(std::unordered_map<ENTITY_ID, Component *> &map : components)
  {
    std::unordered_map<ENTITY_ID, Component *>::iterator component 
      = map.find(id);
    // Delete and remove any found components
    if(component != map.end())
    {
      delete component->second;
      map.erase(component);
    }
  }
}

/*!
 *  Handles removing and deleting all components 
 *  and all entities to clear the ECS.
 */
void ECS::ClearEntities()
{
  for(const ENTITY_ID &entity : entities)
  {
    RemoveEntityComponents(entity);
  }

  entities.clear();
}

/*!
 *  Handles the creation and intalization of the default world singleton 
 *  which tracks global behaviors such as input
 *
 *  TODO: Move input bindings into a key bindings script which can be loaded
 *  on launch
 *
 *  TODO: Create a function which loads the key binding script allowing for
 *  reloading on edit/save
 */
void ECS::AddWorldSingletonComponents()
{
  KeyBindContainer *container = KeyBindContainer::GetInstance();
  // TODO: Create a json file to store keybind data
  container->BindKey(KeyBindContainer::KEY_W
                      , KeyBindContainer::KEY_STATUS_DOWN, "MovePositiveY");
  container->BindKey(KeyBindContainer::KEY_S
                      , KeyBindContainer::KEY_STATUS_DOWN, "MoveNegativeY");
  container->BindKey(KeyBindContainer::KEY_D
                      , KeyBindContainer::KEY_STATUS_DOWN, "MovePositiveX");
  container->BindKey(KeyBindContainer::KEY_A
                      , KeyBindContainer::KEY_STATUS_DOWN, "MoveNegativeX");
  container->BindKey(KeyBindContainer::KEY_I
                      , KeyBindContainer::KEY_STATUS_DOWN, "MovePositiveZ");
  container->BindKey(KeyBindContainer::KEY_K
                      , KeyBindContainer::KEY_STATUS_DOWN, "MoveNegativeZ");
  container->BindKey(KeyBindContainer::KEY_U
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotatePositiveX");
  container->BindKey(KeyBindContainer::KEY_O
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotateNegativeX");
  container->BindKey(KeyBindContainer::KEY_J
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotatePositiveY");
  container->BindKey(KeyBindContainer::KEY_L
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotateNegativeY");
  container->BindKey(KeyBindContainer::KEY_Q
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotatePositiveZ");
  container->BindKey(KeyBindContainer::KEY_E
                      , KeyBindContainer::KEY_STATUS_DOWN, "RotateNegativeZ");

  AddComponent(worldID, Component::C_INPUT, container);
}
