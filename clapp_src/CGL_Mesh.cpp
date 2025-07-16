/*!
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_Mesh.cpp
 *
 *  \brief
 *    The implementation file for the base class identity of what a mesh
 *    is within the Clarity Graphics Library
 */
#include "clapp_includes/pch.h"

#include "clapp_includes/CGL_Mesh.h"

#include "clapp_includes/g_pch.h"

#include "clapp_includes/Clarity_IO.h"
#include "clapp_includes/Clarity_Library.h"

// NOTE: This is a temp include for the graphics system to make sure the
// shaders and mesh work
#include "clapp_includes/CGL_System.h"

using namespace ClaPP;
using namespace std;

Mesh::Mesh(const Mesh::MeshType &type, const std::string &name) {
  switch (type) {
  case Mesh::MESH_TRIANGLE:
    meshData = Library<MeshData>::GetInstance().GetItem("TriangleMesh");
    break;
  case Mesh::MESH_SQUARE:
    meshData = Library<MeshData>::GetInstance().GetItem("SquareMesh");
    break;
  case Mesh::MESH_PRISM:
    meshData = Library<MeshData>::GetInstance().GetItem("PrismMesh");
    break;
  case Mesh::MESH_CUBE:
    meshData = Library<MeshData>::GetInstance().GetItem("CubeMesh");
    break;
  case Mesh::MESH_CUSTOM:
    meshData = Library<MeshData>::GetInstance().GetItem(name);
    break;
  }

  if (meshData == nullptr) {
    ErrMessage("Cannot find given meshdata with name: " + name +
                   ". meshData is nullptr.",
               Mesh::MESHERR);
  }
}

Mesh::MeshData *Mesh::AddMeshDataToLibrary(const std::string &filePath) {
  MeshData *meshData = new MeshData;

  LuaState &instance = LuaState::GetInstance();

  LuaState::LUA_ERR err;

  // Open the lua file
  err = instance.ExecuteFile(filePath);

  // Get the mesh data
  err = instance.GetGlobal("Mesh");

  // Get the mesh type
  err = instance.GetField("type");

  // Default state is a square mesh
  string type = "SquareMesh";
  // TODO: Add a clear operation to the lua state
  err = instance.GetString(type);

  meshData->name = type;

  // Get the mesh data instance of the library
  Library<Mesh> &library = Library<Mesh>::GetInstance();

  if (library.GetItem(meshData->name) != nullptr) {
    ErrMessage("Attempting to create duplicate mesh with name: " +
                   meshData->name,
               MESHERR);
    return nullptr;
  }

  if (meshData->name == "TriangleMesh") 
  {
    meshData->meshType = MESH_TRIANGLE;
  } 
  else if (meshData->name == "SquareMesh") 
  {
    meshData->meshType = MESH_SQUARE;
  } 
  else if(meshData->name == "PrismMesh") 
  {
    meshData->meshType = MESH_PRISM;
  } 
  else if (meshData->name == "CubeMesh")
  {
    meshData->meshType = MESH_CUBE;
  }
  else 
{
    meshData->meshType = MESH_CUSTOM;
  }

  // Leave type field
  err = instance.Pop();

  // Get the color information
  err = instance.GetField("color");
  // Color will always have 3 values but we will get it anyway and check
  // to ensure there are only 3
  int length;
  err = instance.GetFieldLength(length);
  if (length != 3) {
    ErrMessage("Invalid amount of color values in mesh", MESHERR);
    return nullptr;
  }

  // save the colors in a vec3 for vertices
  glm::vec3 color;

  for (int i = 0; i < length; ++i) {
    err = instance.GetNumber(color[i], i);
  }
  // Leave the color field
  err = instance.Pop();

  // Save them into this position vector
  glm::vec3 position;
  vector<glm::vec3> positions;
  glm::vec2 textureCoord;
  vector<glm::vec2> textureCoords;

  // Get the vertices
  err = instance.GetField("vertices");
  err = instance.GetFieldLength(length);

  for (int i = 0; i < length;)  
  {
    // If there isn't enough verticies for another
    // point then break out of the look
    if(length - i < 3)
    {
      break;
    }

    err = instance.GetNumber(position.x, i++);
    err = instance.GetNumber(position.y, i++);
    err = instance.GetNumber(position.z, i++);
    // Fill both positions
    positions.push_back(position);
  }

  // Leave the vertices field
  err = instance.Pop();

  // Get the uv coords 
  err = instance.GetField("uv");
  err = instance.GetFieldLength(length);

  for (int i = 0; i < length;)  
  {
    // If there isn't enough texture coords for another
    // point then break out of the look
    if(length - i < 2)
    {
      break;
    }

    err = instance.GetNumber(textureCoord.x, i++);
    err = instance.GetNumber(textureCoord.y, i++);
    // Fill both texture coords
    textureCoords.push_back(textureCoord);
  }

  // Leave the uv field
  err = instance.Pop();

  // Set the length to the positions size as it should be the number
  // of vertices in the mesh
  length = positions.size();

  // Validate that the sizes are the number of indices are teh same for 
  // the positions and texture coords
  if(positions.size() != textureCoords.size())
  {
    ErrMessage("Invalid number of texture coords to positions for: "
               + meshData->name, MESHERR);

    // Make sure that the smaller of the two sizes is used to avoid
    // indexing outside of the bounds of the vertex
    positions.size() > textureCoords.size() 
      ? length = positions.size() : length = textureCoords.size();
  }

  // Push back the vertices of the mesh
  for(int i = 0; i < length; ++i)
  {
    meshData->vertices.push_back({positions[i], textureCoords[i], color});
  }

  // Finally get the index information
  err = instance.GetField("indices");
  err = instance.GetFieldLength(length);

  // Save each piece into a unsigned int to be added to the vector
  unsigned int index;

  for (int i = 0; i < length; ++i) {
    err = instance.GetNumber(index, i);
    meshData->indices.push_back(index);
  }

  // Pop the indices
  err = instance.Pop();
  // Pop the mesh global
  err = instance.Pop();
  // Pop and clear the filepath :)
  err = instance.Pop();

  // Create the mesh with the new data!
  CreateMeshData(*meshData);

  Library<MeshData>::GetInstance().AddItem(meshData->name, meshData);

  return meshData;
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &meshData->vao);
  glDeleteBuffers(1, &meshData->ebo);
  glDeleteBuffers(1, &meshData->vbo);

  // We do not delete the meshData as we leave that to the library to handle
  // This allows us to delete individual meshes easily without worry about
  // tracking when to the delete the meshData itself since it's shared

  Message("Mesh Destroyed");
}

const Mesh::MeshData &Mesh::GetMeshData() { return *meshData; }

void Mesh::CreateMeshData(MeshData &meshData) {
  if (meshData.vertices.empty() || meshData.indices.empty()) {
    ErrMessage("Empty list of vertices or indices given", MESHERR);
    return;
  }

  glGenVertexArrays(1, &meshData.vao);
  CGL_System::CheckGLError();
  glBindVertexArray(meshData.vao);

  CGL_System::CheckGLError();
  glGenBuffers(1, &meshData.vbo);
  CGL_System::CheckGLError();
  glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
  CGL_System::CheckGLError();
  glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(Vertex),
               meshData.vertices.data(), GL_STATIC_DRAW);
  CGL_System::CheckGLError();

  glGenBuffers(1, &meshData.ebo);
  CGL_System::CheckGLError();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ebo);
  CGL_System::CheckGLError();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               meshData.indices.size() * sizeof(unsigned int),
               meshData.indices.data(), GL_STATIC_DRAW);
  CGL_System::CheckGLError();

  glBindVertexArray(0);
  CGL_System::CheckGLError();

  Message("Mesh Created");
}
