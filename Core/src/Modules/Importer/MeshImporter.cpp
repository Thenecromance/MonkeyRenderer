//
// Created by Thenecromance on 2023/4/23.
//

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>
#include <flecs.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

#include "Logger.hpp"
#include "MeshComp.hpp"
#include "MeshModule.hpp"
#include "Position.hpp"
#include "RenderComp.hpp"

using namespace Monkey::Component;

void ConvertAIMesh(const aiMesh* mesh, MeshData& out) {
  const bool hasTexCoords = mesh->HasTextureCoords(0);

  auto& srcVertex = out.Vertex;
  auto& srcIndices = out.Indices;
  for (auto i = 0; i != mesh->mNumVertices; i++) {
    const auto v = mesh->mVertices[i];
    const auto n = mesh->mNormals[i];
    const auto t = hasTexCoords ? mesh->mTextureCoords[0][i] : aiVector3D();

    srcVertex.push_back(v.x);
    srcVertex.push_back(v.y);
    srcVertex.push_back(v.z);

    

    srcVertex.push_back(n.x);
    srcVertex.push_back(n.y);
    srcVertex.push_back(n.z);
    
    srcVertex.push_back(t.x);
    srcVertex.push_back( t.y);
  }

  for (auto i = 0; i != mesh->mNumFaces; i++) {
    if (mesh->mFaces[i].mNumIndices != 3) continue;
    for (unsigned j = 0; j != mesh->mFaces[i].mNumIndices; j++)
      srcIndices.push_back(mesh->mFaces[i].mIndices[j]);
  }
}

bool LoadByAssimp(const std::string& path, MeshData& out) {
  const unsigned int flags =
      0 | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate |
      aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights |
      aiProcess_SplitLargeMeshes | aiProcess_ImproveCacheLocality |
      aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates |
      aiProcess_FindInvalidData | aiProcess_GenUVCoords;
  const aiScene* scene = aiImportFile(path.c_str(), flags);
  if (!scene) {
    Logger::get("MeshLoader")->error("{} could not load by assimp", path);
    return false;
  }
  if (!scene->HasMeshes()) {
    Logger::get("MeshLoader")->error("{} does not has any meshes in it ", path);
    return false;
  }

  for (unsigned int i = 0; i != scene->mNumMeshes; i++)
    ConvertAIMesh(scene->mMeshes[i], out);
  Logger::get("MeshLoader")
      ->trace("Load {} : Indices size :{} , VertexSize:{}", path,
              out.Indices.size(), out.Vertex.size());

  return true;
}

void LoadMesh(flecs::entity e, MeshFile& mesh) {
  MeshData data;
  if (LoadByAssimp(mesh.path, data)) {
    e.set<MeshData>({data});
  }
}

void LoadMeshDataToGPU(flecs::entity e, MeshData& data) {
  Mesh mesh;
  mesh.numIndices = data.Indices.size();

  glCreateBuffers(1, &mesh.Indices);
  glNamedBufferStorage(mesh.Indices, data.Indices.size()* sizeof(uint32_t), data.Indices.data(),
                       0);

  glCreateBuffers(1, &mesh.Vertices);
  glNamedBufferStorage(mesh.Vertices, data.Vertex.size()* sizeof(uint32_t), data.Vertex.data(),
                       0);

  //  glCreateBuffers(1,&mesh.Indirect);
  //  glNamedBufferStorage(mesh.Indirect,)

  glCreateVertexArrays(1, &mesh.vao);
  glVertexArrayElementBuffer(mesh.vao, mesh.Indices);
  glVertexArrayVertexBuffer(
      mesh.vao, 0, mesh.Vertices, 0,
      sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2));
  // position
  glEnableVertexArrayAttrib(mesh.vao, 0);
  glVertexArrayAttribFormat(
      mesh.vao, 0, 3, GL_FLOAT, GL_FALSE,
      0);  //(GLuint vaobj, GLuint attribindex, GLint size, GLenum type,
           // GLboolean normalized, GLuint relativeoffset);
  glVertexArrayAttribBinding(mesh.vao, 0, 0);
  // uv
  glEnableVertexArrayAttrib(mesh.vao, 1);
  glVertexArrayAttribFormat(mesh.vao, 1, 2, GL_FLOAT, GL_FALSE,
                            sizeof(glm::vec3));
  glVertexArrayAttribBinding(mesh.vao, 1, 0);
  // normal
  glEnableVertexArrayAttrib(mesh.vao, 2);
  glVertexArrayAttribFormat(mesh.vao, 2, 3, GL_FLOAT, GL_TRUE,
                            sizeof(glm::vec3) + sizeof(glm::vec2));
  glVertexArrayAttribBinding(mesh.vao, 2, 0);

  e.set<Mesh>({mesh});
}

void InitMesh(flecs::entity self, Mesh& mesh) {
  if (self.has<BaseRenderer>() || self.has<ForwardRenderer>() ||
      self.has<DefferedRenderer>()) {
    return;
  }
  self.add<ForwardRenderer>();
  self.add<Position>().add<Rotation>().add<Scale>();
}

void RegisterComponent(world& ecs) {
  //  ecs.component<MeshFile>().member<std::string>("path");

  ecs.component<Mesh>()
      .member<Handle>("vao")
      .member<Handle>("Indices")
      .member<Handle>("Vertices")
      .member<Handle>("Indirect");
}

MeshModule::MeshModule(world& ecs) {
  RegisterComponent(ecs);
  ecs.observer<MeshFile>("observer::MeshFileOnSet")
      .event(flecs::OnSet)
      .each(LoadMesh);

  ecs.observer<MeshData>("observer::MeshDataOnSet")
      .event(flecs::OnSet)
      .each(LoadMeshDataToGPU);

  ecs.observer<Mesh>("observer::MeshOnSet").event(flecs::OnSet).each(InitMesh);
}
