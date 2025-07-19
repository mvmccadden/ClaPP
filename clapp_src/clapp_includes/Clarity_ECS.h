/*!
 *  \author  Manoel McCadden
 *  \date    05-14-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    ECS.h
 *
 *  \brief
 *    An ECS manager that handles the creation, deletion, and managment of
 *    entites, components, and systems
*/
#pragma once

#include <locale>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <utility>

#include "Clarity_IO.h"
#include "Clarity_Memory.h"
#include "Clarity_System.h"
#include "Clarity_Entity.h"
#include "Clarity_Component.h"

#include "CIL_Inputs.h"

namespace ClaPP
{

class ECS : public Clarity_System
{
public:
  /*!
   *  
   */
  enum ECS_ERR
  {
    ECS_NO_ERR = 0
    , ECS_FAILED_TO_CREATE
    , ECS_FAILED_TO_DELETE
    , ECS_FAILED_TO_FIND
  };

  ECS();
  ~ECS();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  // Entity Functions
  /*!
   *  Creates a new entity id that can be used to identity an entity.
   *  Data in components can be added to the entity allowing for different
   *  systems to interact with it.
   *
   *  \returns
   *    The new ENTITY_ID that was just created
   */
  ENTITY_ID CreateEntity();
  /*!
  *  Remove an entity and it's components from all systems, maps, and sets
  *  within the ECS
  *
  *  \param id
  *    The ENTITY_ID being removed from the ECS and all its dependants
  */
  void DeleteEntity(const ENTITY_ID &id);

  // Component Functions
  /*!
   *  This specific instance of AddComponent is for singleton or previously
   *  created components that are being copied/moved ONLY.
   *
   *  Adds a given component to the given entity and stores it within the 
   *  component map until either the entity or component is requested to 
   *  be deleted/removed
   *  
   *  \param id
   *    The id of the entity which will be getting this new component
   *  \param component
   *    A pointer to the allocated component which will be stored in 
   *    the component map
   */
  void AddComponent(const ENTITY_ID &id, Component *component);
  /*!
  *   Adds a given component type to a given entity by creating, allocating
  *   , and storing the component within the component map based on it's type 
  *   and parent entity. 
  *
  *   \param id
  *     The entity that is having a component added to it
  *   \param componentArgs
  *     A recursive set of arguments which correspond to the components
  *     constructor. They must be in the same order as the constructor and
  *     must have the same type for the corresponding parameter in the 
  *     components constructor to be properly created.
  */
  template<typename ComponentType, typename... ComponentArgs>
  void AddComponent(const ENTITY_ID &id
                    , ComponentArgs&&... componentArgs)
  {
    // This is used to ensure that any given component is derived from the
    // base type of Component
    static_assert(std::is_base_of<Component, ComponentType>::value
                  , "Component Type passed is not derived from Component");

    // We pass the variadic arguments given to add component to alloc
    // in order to create and allocate the component in the heap as a pointer
    // NOTE: We may be able to adjust this so that the components are no longer
    // pointers but instead just stored in the map as stack allocated objects
    ComponentType *component = memoryManager.Alloc<ComponentType>(
      std::forward<ComponentArgs>(componentArgs)...);

    // Now that we have allocated a new component we can repeat the steps of 
    // singleton component addition so we can just call that
    AddComponent(id, component);
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
  void RemoveComponent(const ENTITY_ID &id
                       , const Component::COMPONENTS &componentType);

  // Template Functions
  // TODO: look into removing enum and instead using typeid
  /*!
  *  Adds a system to the ECS system list. Goes through active entity set
  *  and adds any entities with matching signatures to the system to the
  *  systems entity set
  *
  *  \param system
  *    The system being added to the ECS
  *
  */
  template<typename T>
  T *GetComponent(const ENTITY_ID &id, const Component::COMPONENTS &type)
  {
    // WARN: No try catch for std::out_of_range in casae the entity ID
    // is outside the bounds/doesn't exist in the map current
    // TODO: Fix above!!!
    // NOTE: We are static casting as the compiler doesn't like template 
    // dynamic casting for abstract types
    return static_cast<T*>(components[type].at(id));
  }


  // System functions
  /*!
   *  A templatized system adder which will allocated and create, a 
   *  system in the ECS manager. This will then call the preconstructed
   *  system AddSystem method to store it.
   *
   *  \param systemArgs
   *    A variadic set of arguments needed to construct a new system of
   *    given system type
   */
  template<typename SystemType, typename... SystemArgs>
  void AddSystem(SystemArgs&&... systemArgs)
  {
    SystemType *system = memoryManager.Alloc<SystemType>(
        std::forward<SystemArgs>(systemArgs)...);

    AddSystem(system);
  }
  /*!
   *  This is the add system method for preconstructed systems that are
   *  either singletons are being moved/copied
   *
   *  Adds a new system to the ECS manager
   *
   *  \param system
   *    A pointer to the heap allocated system that will be added
   *    to the ECS manager
   */
  void AddSystem(Clarity_System *system);

  // Const entity functions
  /*!
  *  Each ECS instance has a set of global variables such as the world which
  *  holds and handles singleton component data such as inputs
  *
  *  \returns
  *    A const reference to the world entity which holds input data
  */
  const ENTITY_ID &GetWorldID();

  // ECS global functions
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
  static ECS *GetActiveECS();

private:
  // NOTE: May make 0u an invalid entity id for error checking...
  inline static ENTITY_ID NEW_ENTITY_ID = 0u;

  const ENTITY_ID worldID;

  std::array<std::unordered_map<ENTITY_ID, Component *>
             , COMPONENT_COUNT> components;
  std::unordered_set<ENTITY_ID> entities;
  std::queue<ENTITY_ID> deletedEntities;
  std::unordered_map<ENTITY_ID, COMPONENT_SIGNATURE> entitySignatures;

  std::vector<Clarity_System *> systems;

  MemoryManager memoryManager;

  inline static ECS *activeECS = nullptr;

  // System Updating
  /*!
  *  Updates all systems to either add, remove, or verify an entites existance
  *  based on the entity's and system's signature
  *
  *  \param id
  *    The entity which is being updated in all the systems
  */
  void UpdateEntityInSystems(const ENTITY_ID &id);
  /*!
  *  Remove a given entity from all systems within the ECS
  *
  *  \param id
  *    The entity which will be removed from all systems
  */
  void RemoveEntityFromAllSystems(const ENTITY_ID &id);

  // Component Updating
  /*!
  *  Handles removing all components of a given entity from the component map
  *
  *  \param id
  *    The id of the entity which will have it's components deleted
  */
  void RemoveEntityComponents(const ENTITY_ID &id);

  // Cleanup
  /*!
  *  Handles removing and deleting all components 
  *  and all entities to clear the ECS.
  */
  void ClearEntities();

  // Singleton Methods
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
  void AddWorldSingletonComponents();
};
}
