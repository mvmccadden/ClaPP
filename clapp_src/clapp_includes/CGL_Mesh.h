/*!
 *  \author  Manoel McCadden
 *  \date    05-02-2025
 *  \par     manoel.mccadden@gmail.com
 *  \par     github.com/mvmccadden
 *
 *  \file    CGL_MeshData.h
 *
 *  \brief
 *    The interface file for the base class identity of what a mesh
 *    is within the Clarity Graphics Library
*/

#include <vector>
#include <string>

#include "Clarity_Component.h"

#include "../../external/glm/glm.hpp"

namespace ClaPP
{
struct Vertex
{
  glm::vec3 pos;
  glm::vec2 tex;
  glm::vec3 color;
};

class Mesh : public Component
{
public:
  enum MeshType
  {
    MESH_CUSTOM
    , MESH_TRIANGLE
    , MESH_SQUARE
    , MESH_PRISM
    , MESH_CUBE
  };

  // TODO: Setup proper delete for meshdata
  struct MeshData
  {
    MeshType meshType;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;
    std::string name;
  };

  /*!
  *  Adds given meshdata to the meshdata library using a lua filepath
  *
  *  \param filePath
  *    The filepath to the lua file being used to create the mesh
    */
  static MeshData *AddMeshDataToLibrary(const std::string &filePath);
  
  /*!
   *  One of the mesh CTOR.
   *  Creates a mesh of a given type or name if it is custom
   *
   *  \param type
   *    The type of mesh bein created
   *  \param name
   *    The name of the mesh, used/given only for custom meshes
  */
  Mesh(const MeshType &type, const std::string &name = "");
  ~Mesh();

  const MeshData &GetMeshData();

private:
  MeshData *meshData;

  Mesh() = delete;
  Mesh(const Mesh &other) = delete;
  Mesh &operator=(const Mesh &other);

  // Create a new mesh data instance
  static void CreateMeshData(MeshData &meshData);
};

}
