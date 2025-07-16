/*!
 *  \author  Manoel McCadden
 *  \date    06-14-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par  github.com/mvmccadden
 *
 *  \file    CIL_Controller.h
 *
 *  \brief
 *    The interface file for handling player input for entities
 *    and their controllers
 */

#include "Clarity_Component.h"

namespace ClaPP
{
class Controller : public Component
{
public:
  struct ControllerData
  {
    
  };

  Controller() 
  {

  }
  ~Controller()
  {

  }

  ControllerData &GetControllerData()
  {
    return controllerData;
  }

private:
  ControllerData controllerData;
};
}
