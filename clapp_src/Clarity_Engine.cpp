/*!
 *  \author  Manoel McCadden
 *  \date    07-10-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    ClaPP_Engine.cpp
 *
 *  \brief
 *    Handles engine functionality and implementation for the ClaPP Project
*/

// PCH and H file includes
#include "clapp_includes/pch.h"
#include "clapp_includes/Clarity_Engine.h"

// Base items/utilities included
#include "clapp_includes/Clarity_Component.h"
#include "clapp_includes/Clarity_Entity.h"
#include "clapp_includes/Clarity_System.h"
#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_Memory.h"

// Systems included
#include "clapp_includes/CGL_System.h"
#include "clapp_includes/CPL_System.h"
#include "clapp_includes/CIL_System.h"
#include "clapp_includes/CIL_PlayerControllerSystem.h"

// Components included
#include "clapp_includes/CGL_Mesh.h"
#include "clapp_includes/CGL_Texture.h"
#include "clapp_includes/CPL_Physics.h"
#include "clapp_includes/CPL_Transform.h"
#include "clapp_includes/CIL_Controller.h"



using namespace std;
using namespace ClaPP;

//=================//
//= CTOR and DTOR =//
//=================//

Engine::Engine()
{
  // Adds the default systems to the ecs manager
  ecsManager.AddSystem<CGL_System>("Clarity_Graphics_System");
  ecsManager.AddSystem<Input_System>("Clarity_Input_System");
  ecsManager.AddSystem<PlayerControllerSystem>("Clarity_Control_System");
  ecsManager.AddSystem<CPL_System>("Clarity_Physics_System");
}

Engine::~Engine()
{

}

//==================//
//= Public Methods =//
//==================//

bool Engine::Startup()
{
  if(ecsManager.Initialize() != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }
  if(ecsManager.Load() != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }

  // WARN: THIS IS THE DEFAULT ENTITY CREATION WHICH WILL BE REMOVED
  ENTITY_ID id = ecsManager.CreateEntity();
  ecsManager.AddComponent<Mesh>(id, Mesh::MESH_CUBE);
  // TODO: Fix below note for accuracy in paths
  // NOTE: Since path is relative it must rely on .cpp location...
  // we need to fix this so that it relies on relation to executable
  // or build
  ecsManager.AddComponent<Texture>(id, "../clapp_assets/3D_TEST.png");
  ecsManager.AddComponent<Transform>(id); 
  ecsManager.AddComponent<Controller>(id);
  ecsManager.AddComponent<Physics>(id);

  // TODO: Create a readable lua script that takes in a set of player
  // input events to bind that are easily adjustable
  // ------------------------------
  // |           TABLE            |
  // ------------------------------
  // [KEY | STATUS TRIGGER | EVENT]
  // ------------------------------
  
  return true;
}

bool Engine::Run()
{
  if(TerminateEngine())
  {
    return false;
  }
  if(ecsManager.Update(0.f) != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }
  if(ecsManager.Render() != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }

  // The engine manages the event manage so it will manually update it...
  // TODO: Make event manager a system so it is more integrated into the 
  // lifecycle of the engine
  EventManager::GetInstance().Update(0.f);

  return true;
}

bool Engine::Exit()
{
  if(ecsManager.Unload() != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }
  if(ecsManager.Terminate() != Clarity_System::SYS_NO_ERR)
  {
    return false;
  }

  return true;
}

//===================//
//= Private Methods =//
//===================//

bool Engine::TerminateEngine()
{
  return EventManager::GetInstance().CheckEvent(EventManager::SYSTEM
                                            , EventManager::ENGINE
                                            , "TerminateEngine");
}
