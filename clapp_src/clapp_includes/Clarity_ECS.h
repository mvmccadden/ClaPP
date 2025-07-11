/*
 *  Author:  Manoel McCadden
 *  Data:    05-14-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    ECS.h
 *
 *  Description:
 *    An ECS manager that handles the creation, deletion, and managment of
 *    entites, components, and systems
*/
#pragma once

#include <locale>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "Clarity_IO.h"
#include "Clarity_System.h"
#include "Clarity_Entity.h"
#include "Clarity_Component.h"

#include "CIL_Inputs.h"

namespace ClaPP
{

class ECS : public Clarity_System
{
public:
  ECS();
  ~ECS();

  SYS_ERR Initialize();
  SYS_ERR Load();
  SYS_ERR Update(float deltaTime);
  SYS_ERR Render();
  SYS_ERR Unload();
  SYS_ERR Terminate();

  // Entity Functions
  ENTITY_ID CreateEntity();
  void DeleteEntity(const ENTITY_ID &id);

  // Component Functions
  void AddComponent(const ENTITY_ID &id
                    , const Component::COMPONENTS &componentType
                    , Component *component);
  void RemoveComponent(const ENTITY_ID &id
                       , const Component::COMPONENTS &componentType);

  // Template Functions
  // TODO: look into removing enum and instead using typeid
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
  void AddSystem(Clarity_System *system);

  // Const entity functions
  const ENTITY_ID &GetWorldID();

  // ECS global functions
  static ECS *GetActiveECS();

private:
  // NOTE: May make 0u an invalid entity id for error checking...
  inline static ENTITY_ID NEW_ENTITY_ID = 0u;

  const ENTITY_ID worldID;

  std::array<std::unordered_map<ENTITY_ID, Component *>
             , COMPONENT_COUNT> components;
  std::unordered_set<ENTITY_ID> entities;
  std::queue<ENTITY_ID> deletedEntities;
  std::unordered_map<ENTITY_ID, COMPONENT_SIGNATURE> componentSignatures;

  std::vector<Clarity_System *> systems;

  inline static ECS *activeECS = nullptr;

  const static inline uint64_t ECSERR = 54235423;

  // System Updating
  void UpdateEntityInSystems(const ENTITY_ID &id);
  void RemoveEntityFromAllSystems(const ENTITY_ID &id);

  // Component Updating
  void RemoveEntityComponents(const ENTITY_ID &id);

  // Singleton Methods
  void AddWorldSingletonComponents();
};
}
