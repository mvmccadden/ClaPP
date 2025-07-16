/*!
 *  \author  Manoel McCadden
 *  \date    05-20-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CIL_System.cpp
 *
 *  \brief
 *    The implementation file for interacting with the clarity input system
 */
#include "clapp_includes/pch.h"
#include "clapp_includes/g_pch.h"
#include "clapp_includes/CIL_System.h"

#include "clapp_includes/Clarity_Entity.h"
#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_System.h"
#include "clapp_includes/Clarity_Component.h"
#include "clapp_includes/Clarity_ECS.h"
#include <GLFW/glfw3.h>

using namespace ClaPP;
using namespace std;

Input_System::Input_System(const std::string &_sysName)
: Clarity_System(_sysName)
{
  // As no entites should have an input system we will keep this not
  // to make sure only input components are added but to avoid adding
  // any other componenets and making the size of this system too big
  // TODO: Replaced with an C_INVALID or something of the sort
  systemSignature.set(Component::C_INPUT);
}

Input_System::~Input_System()
{

}
Input_System::SYS_ERR Input_System::Initialize()
{
  vector<LuaState::EnumPair> statusPairs;
  // Add all the status values to the lua table
  statusPairs.push_back({"None", KeyBindContainer::KEY_STATUS_NONE});
  statusPairs.push_back({"Triggered", KeyBindContainer::KEY_STATUS_TRIGGERED});
  statusPairs.push_back({"Held", KeyBindContainer::KEY_STATUS_HELD});
  statusPairs.push_back({"Down", KeyBindContainer::KEY_STATUS_DOWN});
  statusPairs.push_back({"Released", KeyBindContainer::KEY_STATUS_RELEASED});
  // Push status tables
  LuaState::GetInstance().CreateEnum("KeyStatus", statusPairs);


  vector<LuaState::EnumPair> keyPairs;
  // Add all key pairs
  keyPairs.push_back({"A", KeyBindContainer::KEY_A});
  keyPairs.push_back({"B", KeyBindContainer::KEY_B});
  keyPairs.push_back({"C", KeyBindContainer::KEY_C});
  keyPairs.push_back({"D", KeyBindContainer::KEY_D});
  keyPairs.push_back({"E", KeyBindContainer::KEY_E});
  keyPairs.push_back({"F", KeyBindContainer::KEY_F});
  keyPairs.push_back({"G", KeyBindContainer::KEY_G});
  keyPairs.push_back({"H", KeyBindContainer::KEY_H});
  keyPairs.push_back({"I", KeyBindContainer::KEY_I});
  keyPairs.push_back({"J", KeyBindContainer::KEY_J});
  keyPairs.push_back({"K", KeyBindContainer::KEY_K});
  keyPairs.push_back({"L", KeyBindContainer::KEY_L});
  keyPairs.push_back({"M", KeyBindContainer::KEY_M});
  keyPairs.push_back({"N", KeyBindContainer::KEY_N});
  keyPairs.push_back({"O", KeyBindContainer::KEY_O});
  keyPairs.push_back({"P", KeyBindContainer::KEY_P});
  keyPairs.push_back({"Q", KeyBindContainer::KEY_Q});
  keyPairs.push_back({"R", KeyBindContainer::KEY_R});
  keyPairs.push_back({"S", KeyBindContainer::KEY_S});
  keyPairs.push_back({"T", KeyBindContainer::KEY_T});
  keyPairs.push_back({"U", KeyBindContainer::KEY_U});
  keyPairs.push_back({"V", KeyBindContainer::KEY_V});
  keyPairs.push_back({"W", KeyBindContainer::KEY_W});
  keyPairs.push_back({"X", KeyBindContainer::KEY_X});
  keyPairs.push_back({"Y", KeyBindContainer::KEY_Y});
  keyPairs.push_back({"Z", KeyBindContainer::KEY_Z});
  // Create table for keys
  LuaState::GetInstance().CreateEnum("Keys", keyPairs);

  return SYS_NO_ERR;
}

Input_System::SYS_ERR Input_System::Load()
{
  return SYS_NO_ERR;
}

Input_System::SYS_ERR Input_System::Update(float deltaTime)
{
  ECS *ecs = GetECSPtr();
  GLFWwindow *window = glfwGetCurrentContext();
  
  // Single only the worl will ever have the keybind container we only
  // need to update it
  KeyBindContainer *container =
    ecs->GetComponent<KeyBindContainer>(ecs->GetWorldID()
                                        , Component::C_INPUT);
  // We must make sure that it has been properly created
  if(!container)
  {
    ErrMessage("Failed to get input component for world", INPUTERR);
    return SYS_FAILED_TO_UPDATE;
  }

  // Clear the event queue
  container->ClearTriggeredEvents();
  // For every key input the 
  for(std::map<KeyBindContainer::KEYS
      , KeyBindContainer::KeyBind>::iterator it 
      = container->GetKeyBindBegin(); it != container->GetKeyBindEnd(); ++it)
  {
    // TODO: Figure out a good way to override keys
    switch(glfwGetKey(window, KeyBindContainer::ConvertToGLFWKey(it->first)))
    {
    // If the key is currently pressed then either set it to held or set it
    // as triggered
    case GLFW_PRESS:
      it->second.currentStatus = it->second.currentStatus 
          == KeyBindContainer::KEY_STATUS_TRIGGERED 
          ? KeyBindContainer::KEY_STATUS_HELD 
          : KeyBindContainer::KEY_STATUS_TRIGGERED;
      break;
    case GLFW_REPEAT:
      it->second.currentStatus = KeyBindContainer::KEY_STATUS_HELD;
      break;
    // Set the status to released if glfw notates release 
    case GLFW_RELEASE:
      it->second.currentStatus = KeyBindContainer::KEY_STATUS_RELEASED;
      break;
    default:
        it->second.currentStatus = KeyBindContainer::KEY_STATUS_NONE;
    }
    // Queue any events that can be triggered
    if(it->second.currentStatus == it->second.triggerStatus)
    {
      container->TriggerEvent(it->second.keyEvent);
    }
    // TODO: Make a better way of detecting down status
    else if(it->second.triggerStatus == KeyBindContainer::KEY_STATUS_DOWN
        && ((it->second.currentStatus 
        == KeyBindContainer::KEY_STATUS_TRIGGERED)
        || (it->second.currentStatus
        == KeyBindContainer::KEY_STATUS_HELD)))
    {
      container->TriggerEvent(it->second.keyEvent);
    }
  }

  return SYS_NO_ERR;
}

Input_System::SYS_ERR Input_System::Render()
{
  return SYS_NO_ERR;
}

Input_System::SYS_ERR Input_System::Unload()
{
  return SYS_NO_ERR;
}

Input_System::SYS_ERR Input_System::Terminate()
{
  return SYS_NO_ERR;
}
