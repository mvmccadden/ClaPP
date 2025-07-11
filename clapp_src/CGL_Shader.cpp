/*
 * Author:  Manoel McCadden
 * Data:    05-05-2025
 * Email:   manoel.mccadden@gmail.com
 * Github:  github.com/mvmccadden
 *
 * File:    CGL_Shader.cpp
 *
 * Description:
 *  The implementation file for handing shader data in the CGL library
 */
#include "clapp_includes/pch.h"

#include "clapp_includes/CGL_Shader.h"
#include "clapp_includes/CGL_System.h"
#include "clapp_includes/Clarity_IO.h"

#include "clapp_includes/g_pch.h"

using namespace std;
using namespace ClaPP;

CGL_Shader::CGL_Shader(SHADERTYPE type, const char *_shaderPath)
: shaderPath(_shaderPath), shaderType(type), shaderID(0)
  , sharedCount(new unsigned int(1))
{
  if (!_shaderPath) 
  {
    ErrMessage("NULL passed for shader path", SHADERNOTFOUND);
    return;
  }
}

CGL_Shader::CGL_Shader(const CGL_Shader &other) 
: shaderPath(other.shaderPath), shaderType(other.shaderType)
, shaderID(other.shaderID), sharedCount(other.sharedCount)
{ 

}

CGL_Shader &CGL_Shader::operator=(const CGL_Shader &other)
{
  DeleteShader();

  if(other.shaderType != shaderType)
  {
    ErrMessage("Attempting to copy shader of opposite type", SHADERNOTFOUND);
  }
  shaderPath = other.shaderPath;
  shaderID = other.shaderID;
  sharedCount = other.sharedCount;

  return *this;
}

CGL_Shader::~CGL_Shader() 
{
  DeleteShader();
  // Free the memory if the shared count is 0 and there is no
  // possibility of another compiliation
  if(*sharedCount == 0)
  {
    delete sharedCount;
  }
}

void CGL_Shader::DeleteShader()
{
  if(shaderID && --(*sharedCount) == 0)
  {
    glDeleteShader(shaderID);
    CGL_System::CheckGLError();
    shaderID = 0;

    Message("Delete Shader");
  }
}

bool CGL_Shader::CompileShader() 
{
  GLenum err;

  // Create a shader based on the given type
  if (shaderType == VERTEX) 
  {
    shaderID = glCreateShader(GL_VERTEX_SHADER);
  } else if (shaderType == FRAGMENT) 
  {
    shaderID = glCreateShader(GL_FRAGMENT_SHADER);
  } else 
  {
    ErrMessage("Attempting to compile unknown shader type", 12351235);
    return false;
  }
  CGL_System::CheckGLError();

  // Open the file and convert its data to a string to be read
  std::fstream shaderFile;
  OpenFile(shaderFile, shaderPath);
  std::string shaderData;
  ConvertFileToString(shaderFile, shaderData);

  // Read the shader data into a c style string and compile the shader
  const char *shaderCharString = shaderData.c_str();
  glShaderSource(shaderID, 1, &shaderCharString, NULL);
  CGL_System::CheckGLError();
  glCompileShader(shaderID);
  CGL_System::CheckGLError();
  // Error check to make sure the compilation worked
  int status;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
  if(!status)
  {
    char infoLog[512];
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    ErrMessage(infoLog, VERTEXERR);
    return false;
  }
  CGL_System::CheckGLError();

  Message("Compiled Shader from path: " + shaderPath);

  return true;
}

const unsigned int &CGL_Shader::GetShaderID() 
{ 
  return shaderID; 
}

CGL_Program::CGL_Program(const CGL_Shader &vertexShader
                         , const CGL_Shader &fragmentShader)
: vertex(vertexShader), fragment(fragmentShader), programID(0) 
{
  if (vertex.shaderType != CGL_Shader::VERTEX) 
  {
    ErrMessage("Incorrect shader types passed for vertex shader",
               CGL_Shader::VERTEXERR);
    return;
  }
  if (fragment.shaderType != CGL_Shader::FRAGMENT) 
  {
    ErrMessage("Incorrect shader types passed for fragment shader",
               CGL_Shader::FRAGMENTERR);
    return;
  }

  LinkProgram();
}

CGL_Program::~CGL_Program() 
{
  DeleteProgram();
}

void CGL_Program::DeleteProgram()
{
  if(programID)
  {
    glDeleteProgram(programID);
    CGL_System::CheckGLError();
    programID = 0;

    Message("Delete Program");
  }
}

bool CGL_Program::LinkProgram() 
{
  // Recompile shaders or compile them for the first time
  vertex.CompileShader();
  fragment.CompileShader();

  programID = glCreateProgram();
  CGL_System::CheckGLError();
  glAttachShader(programID, vertex.GetShaderID());
  CGL_System::CheckGLError();
  glAttachShader(programID, fragment.GetShaderID());
  CGL_System::CheckGLError();
  glLinkProgram(programID);
  CGL_System::CheckGLError();
  int status;
  glGetProgramiv(programID, GL_LINK_STATUS, &status);
  if(!status)
  {
    char infoLog[512];
    glGetProgramInfoLog(programID, 512, NULL, infoLog);
    ErrMessage(infoLog, PROGRAMERR);
    return false;
  }
  CGL_System::CheckGLError();

  inColor = static_cast<unsigned int>(glGetAttribLocation(programID, "inColor"));
  CGL_System::CheckGLError();
  if(inColor == -1)
  {
    ErrMessage("Failed to find inColor in shader", PROGRAMERR);
  }
  inPos = static_cast<unsigned int>(glGetAttribLocation(programID, "inPos"));
  CGL_System::CheckGLError();
  if(inPos == -1)
  {
    ErrMessage("Failed to find inPos in shader", PROGRAMERR);
  }
  inTex = static_cast<unsigned int>(glGetAttribLocation(programID, "inTex"));
  CGL_System::CheckGLError();
  if(inTex == -1)
  {
    ErrMessage("Failed to find inTex in shader", PROGRAMERR);
  }

  objMatrix = static_cast<unsigned int>(
    glGetUniformLocation(programID, "obj"));
  CGL_System::CheckGLError();
  if(objMatrix == -1)
  {
    ErrMessage("Failed to find obj in shader", PROGRAMERR);
  }
  viewMatrix = static_cast<unsigned int>(
    glGetUniformLocation(programID, "view"));
  CGL_System::CheckGLError();
  if(viewMatrix == -1)
  {
    ErrMessage("Failed to find view in shader", PROGRAMERR);
  }
  perspectiveMatrix = static_cast<unsigned int>(
    glGetUniformLocation(programID, "perspective"));
  CGL_System::CheckGLError();
  if(perspectiveMatrix == -1)
  {
    ErrMessage("Failed to find perspective in shader", PROGRAMERR);
  }

  Message("Compiled Program");

  return true;
}

const unsigned int &CGL_Program::GetColorLocation()
{
  return inColor;
}

const unsigned int &CGL_Program::GetPositionLocation()
{
  return inPos;
}

const unsigned int &CGL_Program::GetTextureLocation()
{
  return inTex;
}

const unsigned int &CGL_Program::GetObjMatrixLocation()
{
  return objMatrix;
}

const unsigned int &CGL_Program::GetViewMatrixLocation()
{
  return viewMatrix;
}

const unsigned int &CGL_Program::GetPerspectiveMatrixLocation()
{
  return perspectiveMatrix;
}

const unsigned int &CGL_Program::GetProgramID() 
{ 
  return programID; 
}
