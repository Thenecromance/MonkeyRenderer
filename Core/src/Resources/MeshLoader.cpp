//
// Created by Thenecromance on 2023/4/6.
//
#include "MeshLoader.hpp"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <string>

#include "Logger.hpp"

using namespace glm;

#pragma region MeshLoader

void MeshLoader::LoadWithAssimp(const std::string &file_path,
                                std::vector<VertexData> &out_vertex,
                                std::vector<unsigned int> &out_indices) {
  static aiLogStream stream;
  stream.callback = [](const char *msg, char *user) {
    Logger::get("Assimp")->error("{}", msg);
  };
  aiAttachLogStream(&stream);
  const aiScene *scene = aiImportFile(file_path.c_str(), aiProcess_Triangulate);

  if (!scene) {
    Logger::get("MeshLoader")
        ->error("Unable to load file: {}, assimp error:{}", file_path,
                aiGetErrorString());
    aiReleaseImport(scene);
    exit(255);
  }
  if (!scene->HasMeshes()) {
    Logger::get("MeshLoader")->error("File does not has meshes: {}", file_path);
    aiReleaseImport(scene);
    exit(255);
  }

  std::vector<VertexData> vertices;
  std::vector<unsigned int> indices;

  //	const aiMesh *mesh = scene->mMeshes[0];

  for (int m = 0; m < scene->mNumMeshes; ++m) {
    const aiMesh *mesh = scene->mMeshes[m];

    for (unsigned int i = 0; i != mesh->mNumVertices; i++) {
      const aiVector3D v = mesh->mVertices[i];
      const aiVector3D n = mesh->mNormals[i];
      const aiVector3D t = mesh->mTextureCoords[0][i];
      vertices.push_back({.pos = vec3(v.x, v.y, v.z),
                          .n = vec3(n.x, n.y, n.z),
                          .tc = vec2(t.x, 1.0f - t.y)});
    }

    for (unsigned int i = 0; i != mesh->mNumFaces; i++) {
      for (unsigned int j = 0; j != 3; j++) {
        indices.push_back(mesh->mFaces[i].mIndices[j]);
      }
    }
  }

  out_vertex.resize(vertices.size());
  out_indices.resize(indices.size());
  std::copy(vertices.begin(), vertices.end(), out_vertex.begin());
  std::copy(indices.begin(), indices.end(), out_indices.begin());

  aiReleaseImport(scene);
}

#pragma endregion

#pragma region ModelFinder

void ModelFinder::UpdateNameAndID(const std::string &name, const uint64_t &id) {
  mModelNameToId[name] = id;
  mModelIdToName[id] = name;
}

uint64_t ModelFinder::GetIdByName(const std::string &name) {
  if (mModelNameToId.find(name) == mModelNameToId.end()) return 0;
  return mModelNameToId[name];
}

std::string ModelFinder::GetNameById(const uint64_t &id) {
  if (mModelIdToName.find(id) == mModelIdToName.end()) return {};
  return mModelIdToName[id];
}

bool ModelFinder::Erase(uint64_t id) {
  if (mModelIdToName.find(id) == mModelIdToName.end()) {
    Logger::get<ModelFinder>()->error("Can't find id: {} in ModelFinder", id);
    return false;
  }
  auto name = mModelIdToName[id];
  mModelIdToName.erase(id);
  mModelNameToId.erase(name);
  return true;
}

bool ModelFinder::Erase(const std::string &name) {
  if (mModelNameToId.find(name) == mModelNameToId.end()) {
    Logger::get<ModelFinder>()->error("Can't find name: {} in ModelFinder",
                                      name);
    return false;
  }
  auto id = mModelNameToId[name];
  mModelNameToId.erase(name);
  mModelIdToName.erase(id);
  return true;
}

#pragma endregion

#include "Buffer.hpp"
#include "glad/glad.h"

#pragma region ModelLoader

uint64_t ModelLoader::GetId(const std::string &name) {
  return finder.GetIdByName(name);
}

std::string ModelLoader::GetName(const uint64_t &id) {
  return finder.GetNameById(id);
}

Handle ModelLoader::GetVaoHandle(const uint64_t &id) {
  return mDataHandles[id].vao;
}

Handle ModelLoader::GetVaoHandle(const std::string &name) {
  return GetVaoHandle(GetId(name));
}

Handle ModelLoader::GetVboHandle(const uint64_t &id) {
  if (id == 0) return 0;
  return mDataHandles[id].vbo;
}

Handle ModelLoader::GetVboHandle(const std::string &name) {
  return GetVboHandle(GetId(name));
}

UID ModelLoader::LoadModelAndUpLoad(const std::string &file_path,
                                    const std::string &name) {
  auto uid = LoadModelButNotUpLoad(file_path, name);
  if (UpLoadToGPU(uid)) return uid;
  return 0;
}

UID ModelLoader::LoadModelButNotUpLoad(const std::string &file_path,
                                       const std::string &name) {
  UID uid = mIdGenerator.next_id();
  std::string name_ = name;
  if (name_.empty()) {
    UseFileNameAsName(file_path, name_);
  }

  MeshLoader::GetInstance()->LoadWithAssimp(file_path, mMeshData[uid].vertex,
                                            mMeshData[uid].indices);

  Logger::get<ModelLoader>()->trace("Load model: {} with id: {}", name_, uid);
  UpdateNameToId(name_, uid);
  UpdateFile(file_path, uid);
  return uid;
}

void ModelLoader::UpdateNameToId(const std::string &name, uint64_t &id) {
  finder.UpdateNameAndID(name, id);
}

void ModelLoader::UpdateFile(const std::string &file_path, uint64_t &id) {
  mModelFilePath[id] = file_path;
}

void ModelLoader::UseFileNameAsName(const std::string &file_path,
                                    std::string &name) {
  auto pos = file_path.find_last_of('/');
  if (pos == std::string::npos) {
    pos = file_path.find_last_of('\\');
  }
  if (pos == std::string::npos) {
    name = file_path;
  } else {
    name = file_path.substr(pos + 1);
  }
}

bool ModelLoader::UpLoadToGPU(uint64_t &id) {
  if (!CreateBuffer(id)) {
    Logger::get<ModelLoader>()->error(
        "Buffer create failed, unable to upload model to gpu");
    return false;
  }

  return true;
}

bool ModelLoader::CreateBuffer(uint64_t &id) {
  glCreateVertexArrays(1, &mDataHandles[id].vao);
  if (mDataHandles[id].vao == 0) {
    Logger::get<ModelLoader>()->error("Create vao failed");
    return false;
  }
  Logger::get<ModelLoader>()->trace("Create vao: {}", mDataHandles[id].vao);

  Handle IndicesHandle;
  {
    IndicesHandle = mGBuffers[id].gIndices.Update(
        mMeshData[id].indices.size() * sizeof(unsigned int),
        mMeshData[id].indices.data(), 0);
    Logger::get<ModelLoader>()->trace("Create indices buffer: {}",
                                      IndicesHandle);
  }

  mDataHandles[id].vbo = mGBuffers[id].gVertex.Update(
      mMeshData[id].vertex.size() * sizeof(VertexData),
      mMeshData[id].vertex.data(), 0);
  Logger::get<ModelLoader>()->trace("Create vertex buffer: {}",
                                    mDataHandles[id].vbo);

  glBindVertexArray(mDataHandles[id].vao);
  glVertexArrayElementBuffer(mDataHandles[id].vao, IndicesHandle);

  return true;
}

unsigned int ModelLoader::GetIndicesCount(const uint64_t &id) {
  //	return mGBuffers[id].gIndices.size();
  return mMeshData[id].indices.size();
}

unsigned int ModelLoader::GetIndicesCount(const std::string &name) {
  return GetIndicesCount(GetId(name));
}

#pragma endregion