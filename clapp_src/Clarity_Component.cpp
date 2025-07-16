/*!
 *  \author  Manoel McCadden
 *  \date    05-14-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    Clarity_Component.h
 *
 *  \brief
 *    An abstract class used to define what a component is
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Component.h"

using namespace std;
using namespace ClaPP;

Component::Component() 
{

}

Component::~Component()
{

}

const std::string &Component::ToString(const COMPONENTS &component)
{
  return componentMap[static_cast<int>(component)];
}

Component::COMPONENTS Component::ToEnum()
{
  std::string typeName = typeid(*this).name();
                                               
  for(int i = 0; i < C_COUNT; ++i)
  {
    if(componentMap[i] == typeName)
    {
      return static_cast<COMPONENTS>(i);
    }
  }
                                               
  return C_INVALID;
}
