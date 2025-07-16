/*!
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_System.cpp
 *
 *  \brief
 *    The implementation file for the system of the Clarity Graphics Library
 *    (CGL). This library provides a sort of wrapper over the base implementation 
 *    needed to run glfw/glad.
*/
#include "clapp_includes/pch.h"

#include "clapp_includes/Clarity_Component.h"
#include "clapp_includes/Clarity_Entity.h"

#include "clapp_includes/CGL_System.h"

#include "clapp_includes/Clarity_Memory.h"
#include "clapp_includes/Clarity_System.h"
#include "clapp_includes/Clarity_IO.h" 
#include "clapp_includes/CGL_Shader.h"
#include "clapp_includes/CGL_Mesh.h"
#include "clapp_includes/CGL_Texture.h"
#include "clapp_includes/CPL_Transform.h"

#include "clapp_includes/Clarity_ECS.h"

#include "clapp_includes/g_pch.h"

using namespace std;
using namespace ClaPP;

/*
 * Definition of callback function for resizing framebuffer
 */
static void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
  // Retrieve the class as a whole so we can call a custom class-specific callback
  CGL_System *sys = static_cast<CGL_System*>(glfwGetWindowUserPointer(window));
  
  if(!sys)
  {
    ErrMessage("FAILED TO RETRIEVE WINDOW USER POINTER", CGL_System::WINDOWERR);
    return;
  }

  sys->CGL_FramebufferSizeCallback(width, height);
}

/*
 * Implement what a window container holds
 */
struct ClaPP::CGL_System::WindowContainer
{
  GLFWwindow *window;
  int width, height;
};

/*
 * Implementation of Public CGL Functions
*/

CGL_System::CGL_System(const std::string &_sysName, uint32_t _windowSettings) 
: Clarity_System(_sysName), windowData(new WindowContainer) 
, windowSettings(_windowSettings), defaultShader(nullptr)
{
  // Set component signature
  systemSignature.set(static_cast<size_t>(Component::C_MESH));
  systemSignature.set(static_cast<size_t>(Component::C_TEXTURE));
  systemSignature.set(static_cast<size_t>(Component::C_TRANSFORM));
}

CGL_System::~CGL_System() {}

bool CGL_System::CheckWindowClose()
{
  return glfwWindowShouldClose(windowData->window);
}

CGL_System::SYS_ERR CGL_System::Initialize()
{
  Message(GetSysName() + " Initializing");

  if(!glfwInit())
  {
    ErrMessage(GetSysName() + "Failed to init GLFW", GLFWERR);
    return SYS_FAILED_TO_INITIALIZE;
  }

  glfwSetErrorCallback(GraphicsError);
  
  ProcessWindowHints();

  windowData->window = glfwCreateWindow(1280, 720, GetSysName().c_str()
                                        , nullptr, nullptr);

  if(!windowData->window)
  {
    ErrMessage(GetSysName() + "Failed to create GLFW window", WINDOWERR);
    glfwTerminate();
    return SYS_FAILED_TO_INITIALIZE;
  }

  glfwMakeContextCurrent(windowData->window);

  // Set the window user pointer so we can easily retrieve this
  // class when needed
  glfwSetWindowUserPointer(windowData->window, this);

  InitOpenGL();

  // Set buffer callback
  glfwSetFramebufferSizeCallback(windowData->window, FramebufferSizeCallback);

  // Set the inital frame buffer size
  glfwGetFramebufferSize(windowData->window, &windowData->width, &windowData->height);
  glViewport(0, 0, windowData->width, windowData->height);

  // Setup 3D depth
  glEnable(GL_DEPTH_TEST);

  // Setup shaders
  defaultShader = new CGL_Program((CGL_Shader) {CGL_Shader::VERTEX
                                  , "../clapp_shaders/vertShader.txt"}
                                  , (CGL_Shader) {CGL_Shader::FRAGMENT
                                  , "../clapp_shaders/fragShader.txt"});

  // Change this to bind all meshes in scripts
  Mesh::MeshData *meshData = Mesh::AddMeshDataToLibrary(
    "../clapp_scripts/meshes/TriangleMesh.lua");
  // WARN: Currently only cube is being bound to shader
  meshData = Mesh::AddMeshDataToLibrary(
    "../clapp_scripts/meshes/CubeMesh.lua");

  // Get the shader's locations
  unsigned int inColor = defaultShader->GetColorLocation();
  unsigned int inPos = defaultShader->GetPositionLocation();
  unsigned int inUV = defaultShader->GetTextureLocation();

  glBindVertexArray(meshData->vao);

  glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE
                        , sizeof(Vertex), (void *) offsetof(Vertex, color));
  CGL_System::CheckGLError();
  glEnableVertexAttribArray(inColor);
  CGL_System::CheckGLError();
  glVertexAttribPointer(inUV, 2, GL_FLOAT, GL_FALSE
                        , sizeof(Vertex), (void *) offsetof(Vertex, tex));
  CGL_System::CheckGLError();
  glEnableVertexAttribArray(inUV);
  CGL_System::CheckGLError();
  glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE
                        , sizeof(Vertex), (void *) offsetof(Vertex, pos));
  CGL_System::CheckGLError();
  glEnableVertexAttribArray(inPos);
  CGL_System::CheckGLError();

  glBindVertexArray(0);

  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::Load()
{
  Message(GetSysName() + " Loading");


  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::Update(float dt)
{
  if(glfwWindowShouldClose(windowData->window))
  {
    EventManager::GetInstance().AddEvent(
      EventManager::Event(EventManager::SYSTEM
                          , EventManager::ENGINE
                          , "TerminateEngine"));
  }
  //Message("Entity Count: " + std::to_string(systemEntites.size()));

  glfwPollEvents();


  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::Render()
{
  //Message(GetSysName() + " Graphics Rendering");

  glfwMakeContextCurrent(windowData->window);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  CheckGLError();

  glUseProgram(defaultShader->GetProgramID());
  CheckGLError();

  // Use the ecs system to get the needed componennts
  ECS *ecs = GetECSPtr();

  // render all entites that are held by the graphics system
  for(const ENTITY_ID &entity : systemEntites)
  {
    Mesh *mesh = ecs->GetComponent<Mesh>(entity, Component::C_MESH);

    Texture *texture = ecs->GetComponent<Texture>(entity
                                                  , Component::C_TEXTURE);

    // Temporarly pull the transform and bind it in the render step
    Transform *transform = ecs->GetComponent
      <Transform>(entity, Component::C_TRANSFORM);

    unsigned int objMatrix = defaultShader->GetObjMatrixLocation();
    glUniformMatrix4fv(objMatrix, 1, GL_FALSE
                       , glm::value_ptr(
                       transform->GetTransformData().worldMatrix));
    CheckGLError();

    // Temporarly bind the perspectice and view matrices with each entity
    glm::mat4 view = glm::mat4(1.f);
    view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));
    glm::mat4 perspective;
    perspective = glm::perspective(glm::radians(70.f), 1200.f/720.f
                                   , 0.1f, 100.f);

    unsigned int viewMatrix = defaultShader->GetViewMatrixLocation();
    unsigned int perspectiveMatrix = 
      defaultShader->GetPerspectiveMatrixLocation();

    glUniformMatrix4fv(viewMatrix, 1, GL_FALSE
                       , glm::value_ptr(view));
    CheckGLError();
    glUniformMatrix4fv(perspectiveMatrix, 1, GL_FALSE
                       , glm::value_ptr(perspective));
    CheckGLError();

    // Bind texture for rendering
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureData().textureID);
    CheckGLError();

    glBindVertexArray(mesh->GetMeshData().vao);
    CheckGLError();

    glDrawElements(GL_TRIANGLES
                  , mesh->GetMeshData().indices.size()
                  , GL_UNSIGNED_INT, 0);
    CheckGLError();

    glBindVertexArray(0);
    CheckGLError();
    
  }

  glfwSwapBuffers(windowData->window);

  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::Unload()
{
  Message(GetSysName() + " Graphics Unloading");

  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::Terminate()
{
  delete defaultShader;

  glfwDestroyWindow(windowData->window);
  glfwTerminate();

  delete windowData;
  
  Message(GetSysName() + " Terminating");

  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::InitOpenGL()
{
  if(isOpenGLInitialized)
  {
    return SYS_NO_ERR;
  }

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    cerr << "Failed to initialize GLAD" << endl;
    return SYS_FAILED_TO_INITIALIZE;
  }

  isOpenGLInitialized = true;

  return SYS_NO_ERR;
}

CGL_System::SYS_ERR CGL_System::ProcessWindowHints()
{
  // Used to reset window hints back to their default values
  glfwDefaultWindowHints();

  // Set opengl hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Use the internally save window settings to set the window hints
  // TODO: Implement the window hints

  return SYS_NO_ERR;
}

void CGL_System::GraphicsError(int error, const char *details)
{
  // Append graphics error info onto string to make more clear 
  // when printing error message where it came from
  char *message = new char[strlen(details) + strlen("Graphics Error: ") + 1];
  strcpy(message, "Graphics Error: ");
  strcat(message, details);

  ErrMessage(message, error);
  
  delete[] message;
}

void CGL_System::CheckGLError()
{
  GLenum err = glGetError();
  if(err != GL_NO_ERROR)
  {
    GraphicsError(err, "OPENGL ERROR");
  }
}

void CGL_System::CGL_FramebufferSizeCallback(int width, int height)
{
  windowData->width = width;
  windowData->height = height;

  glViewport(0, 0, windowData->width, windowData->height);

  Message(GetSysName() + " Window Resize");

  // Update shaders and matrices
  // Trigger re-render as neeeded
}
