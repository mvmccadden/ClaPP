/*
 *  \author  Manoel McCadden
 *  \date    07-18-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Events.cpp
 *
 *  \brief
 *    An implementation for event managment
*/

#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_EventManager.h"

namespace ClaPP
{
EventManager::Event::Event(const EVENT_OBJ &_sender
                           , const EVENT_OBJ &_reciever
                           , const std::string &_eventMessage)
: sender(_sender), reciever(_reciever), eventMessage(_eventMessage) {}
// NOTE: Initally doing this as a singleton to simplify but will adjust
// for multi-thread or multi-engine running later
EventManager &EventManager::GetInstance()
{
  static EventManager instance;
  return instance;
}
void EventManager::AddEvent(Event &&event)
{
  events.push_back(event);
}
// Check all events for a specific message from a specific sender
bool EventManager::CheckEvent(const EVENT_OBJ &_desiredSender
                              , const EVENT_OBJ &_reciever
                              , const std::string &_expectedMessage)
{
  for(Event &event : events)
  {
    if(_desiredSender == event.sender && _reciever == event.reciever
    && _expectedMessage == event.eventMessage)
    {
      event.eventChecked = true;
      return true;
    }
  }
  return false;
}
// Remove any events that are checked
void EventManager::Update(float deltaTime)
{
  for(std::vector<Event>::iterator it = events.begin()
  ; it != events.end(); ++it)
  {
    if(it->eventChecked)
    {
      events.erase(it);
    }
  }
}

EventManager::EventManager()
{

}

EventManager::~EventManager()
{

}
}
