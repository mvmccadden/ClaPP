/*
 *  \author  Manoel McCadden
 *  \date    07-18-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_EventManager.h
 *
 *  \brief
 *    An interface for event managment
*/
#pragma once

#include <vector>
#include <string>

namespace ClaPP
{
/*!
 * \class EventManager
 *
 * \brief
 *  A helper class designed to manager events on a global scale.
 *
 *  operations include:
 *  - Adding an event
 *  - Reading events
 */
class EventManager
{
public:
  enum EVENT_OBJ
  {
    COMPONENT = 0
    , ENTITY
    , SYSTEM
    , ENGINE
    , USER_COMPONENT
    , USER_ENTITY
    , USER_SYSTEM

  };

  struct Event
  {
    Event(const EVENT_OBJ &_sender, const EVENT_OBJ &_reciever
          , const std::string &_eventMessage);

    EVENT_OBJ sender;
    EVENT_OBJ reciever;
    std::string eventMessage;
    bool eventChecked = false;
  };
  // NOTE: Initally doing this as a singleton to simplify but will adjust
  // for multi-thread or multi-engine running later
  static EventManager &GetInstance();
  void AddEvent(Event &&event);
  // Check all events for a specific message from a specific sender
  bool CheckEvent(const EVENT_OBJ &_desiredSender
                                , const EVENT_OBJ &_reciever
                                , const std::string &_expectedMessage);

  // Remove any events that are checked
  void Update(float deltaTime);

private:
  EventManager();
  ~EventManager();

  EventManager(const EventManager &other) = delete;
  EventManager &operator=(const EventManager &other) = delete;

  std::vector<Event> events;

  inline static const std::string EMPTY_STRING = "";
};
}
