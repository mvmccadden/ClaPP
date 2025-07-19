/*
 *  \author  Manoel McCadden
 *  \date    05-20-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CIL_System.h
 *
 *  /brief
 *    The interface file for interacting with the clarity input system and
 *    assigning keybinds
 */
#pragma once

#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include "Clarity_Component.h"
#include "Clarity_Entity.h"
#include "Clarity_IO.h"

namespace ClaPP
{
typedef class ECS ECS;

class KeyBindContainer : public Component
{
public:
  // Related glfw key values are placed on the side
  enum KEYS
  {
    KEY_SPACE = 0 //= 32
    , KEY_0 //= 48
    , KEY_1 
    , KEY_2
    , KEY_3
    , KEY_4
    , KEY_5
    , KEY_6
    , KEY_7
    , KEY_8
    , KEY_9
    , KEY_A //= 65
    , KEY_B
    , KEY_C
    , KEY_D
    , KEY_E
    , KEY_F
    , KEY_G
    , KEY_H
    , KEY_I
    , KEY_J
    , KEY_K
    , KEY_L
    , KEY_M
    , KEY_N
    , KEY_O
    , KEY_P
    , KEY_Q
    , KEY_R
    , KEY_S
    , KEY_T
    , KEY_U
    , KEY_V
    , KEY_W
    , KEY_X
    , KEY_Y
    , KEY_Z
    , KEY_F1 //= 290
    , KEY_F2
    , KEY_F3
    , KEY_F4
    , KEY_F5
    , KEY_F6
    , KEY_F7
    , KEY_F8
    , KEY_F9
    , KEY_F10
    , KEY_F11
    , KEY_F12
    , KEY_LEFT_SHIFT //= 340
    , KEY_LEFT_CONTROL
    , KEY_LEFT_ALT
    , KEY_RIGHT_SHIFT //= 344
    , KEY_RIGHT_CONTROL
    , KEY_RIGHT_ALT
    , KEY_COUNT
  };

  enum KEY_STATUS
  {
    KEY_STATUS_NONE = 0
    , KEY_STATUS_TRIGGERED 
    , KEY_STATUS_HELD 
    // For both triggered and held states
    , KEY_STATUS_DOWN 
    , KEY_STATUS_RELEASED 
  };

  typedef std::string KEY_EVENT;

  /*!
   *  \struct KeyBind
   *
   *  \details
   *    A structure that holds keybind information suchs as the key
   *    , status of the key, and an event that will trigger on
   *    a certain status event. 
   *    Currently only 1 event trigger is allowed per key...
   *
   *    TODO: Add possibility for events for each status
  */
  struct KeyBind 
  {
    KEY_STATUS currentStatus = KEY_STATUS_NONE;
    KEY_STATUS triggerStatus;
    KEY_EVENT keyEvent;
  };

  static KeyBindContainer *GetInstance()
  {
    static KeyBindContainer *instance = new KeyBindContainer();
    return instance;
  }

  void BindKey(const KEYS &key, const KEY_STATUS &statusTrigger
               , KEY_EVENT event)
  {
    // Attempt to locate the key
    std::map<KEYS, KeyBind>::iterator it = keyBinds.find(key);

    // We want the key to not exists so that we arn't binding over another key
    // , but if we are then just send a message to warn the user in case it's
    // intentional
    if(it != keyBinds.end())
    {
      Message("Binding new event to already bound key: " + std::to_string(key)
              , SEVERITY_INFO);
    }

    // Bind the key to the new event trigger
    auto emplaced = keyBinds.try_emplace(key
                                         , KeyBind{ KEY_STATUS_NONE
                                          , statusTrigger, event});

    // Check if the bind properly emplaced and report if an error occured
    if(!emplaced.second)
    {
      ErrMessage("Failed to create bind for key: " + std::to_string(key)
                 , EC_INPUT);
    }
  }

  void UnbindKey(const KEYS &key)
  {
    std::map<KEYS, KeyBind>::iterator it = keyBinds.find(key);

    // Make sure the key exists
    if(it == keyBinds.end())
    {
      ErrMessage("Attempting to unbind key that is not bound, key: " 
                 + std::to_string(key), EC_INPUT);

      return;
    }
  
    // Delete the item
    // NOTE: May want to check this value but im not sure how best to do
    // that right now
    keyBinds.erase(it);
  }

  std::map<KEYS, KeyBind>::iterator GetKeyBindBegin()
  {
    return keyBinds.begin();
  }

  std::map<KEYS, KeyBind>::iterator GetKeyBindEnd()
  {
    return keyBinds.end();
  }

  const std::map<KEYS, KeyBind> &GetKeyBindMap()
  {
    return keyBinds;
  }

  void TriggerEvent(const KEY_EVENT &event)
  {
    // Will not duplicate an insert if it exists
    triggeredEvents.insert(event);
  }

  bool CheckEvent(const KEY_EVENT& event)
  {
    return static_cast<bool>(triggeredEvents.count(event));
  }

  void ClearTriggeredEvents()
  {
    triggeredEvents.clear();
  }

  static int ConvertToGLFWKey(const KEYS &key)
  {
    if(key >= KEY_RIGHT_SHIFT)
    {
      return static_cast<int>(key) - static_cast<int>(KEY_RIGHT_SHIFT) + 344;
    }
    else if(key >= KEY_LEFT_SHIFT)
    {
      return static_cast<int>(key) - static_cast<int>(KEY_LEFT_SHIFT) + 340;
    }
    else if(key >= KEY_F1)
    {
      return static_cast<int>(key) - static_cast<int>(KEY_F1) + 290;
    }
    else if(key >= KEY_A)
    {
      return static_cast<int>(key) - static_cast<int>(KEY_A) + 65;
    }
    else if(key >= KEY_0)
    {
      return static_cast<int>(key) - static_cast<int>(KEY_0) + 48;
    }
    else if(key == KEY_SPACE)
    {
      return 32;
    }
    else 
    {
      ErrMessage("Failed to convert key to GLFW Code", EC_INPUT);
      return 0;
    }
  }

private:
  // TODO: Add an array of maps that bind keys based on game "states"
  // such as editor, menu, playing, spectator, ect.
  std::map<KEYS, KeyBind> keyBinds;
  std::unordered_set<KEY_EVENT> triggeredEvents;
  // TODO: Keep track of input type somehow and which binds match
  // to which

  KeyBindContainer() {}
  ~KeyBindContainer() {}
};
}
