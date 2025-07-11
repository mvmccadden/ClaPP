/*
 *  Author:  Manoel McCadden
 *  Data:    06-22-2025
 *  Email:   manoel.mccadden@gmail.com
 *  Github:  github.com/mvmccadden
 *
 *  File:    Clarity_Clock.h
 *
 *  Description:
 *    An interface that handles time keeping mechanics of the engine/system
*/
#pragma once

#include <chrono>
#include <cstdint>

#include "Clarity_System.h"

namespace ClaPP
{
// Static instance of clock which is easily accesable
class Clock 
{
public:
  Clock();
  ~Clock();

  void Update();
  const float &GetDeltaTime() const;
  const float &GetTotalRunTime() const;

private:
  // The time between updates
  float deltaTime = 0.f;
  // The total time from initialization
  float gameRunTime = 0.f;
  // Keeps track of when the previous update call was made to 
  // accuratly get deltaTime
  float previousGameRunTime = 0.f;
};

// Default clock used by the system
// NOTE: Must be created in c++ like a static variable;
extern Clock systemClock;

class Timer
{
public:
  Timer();
  // Allows user to use a custom clockc to override the default clock
  Timer(const Clock &clockOverride);
  ~Timer();
private:
  const Clock& clock;
};

}
