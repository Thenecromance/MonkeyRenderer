#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>

#include "Buffer/Buffer.hpp"
#include "CommonDef.hpp"
#include "Patterns/SingleTon.hpp"
#include "utils/UID/SnowFlake.hpp"

struct VertexData {
  glm::vec3 pos;
  glm::vec3 n;
  glm::vec2 tc;
};

struct MeshData {
  std::vector<VertexData> vertex;
  std::vector<unsigned int> indices;
};


// TODO: MeshLoader still need to working on,when using the same mesh, it should
// not be loaded again or whole this part need to redesign
class MeshLoader : public LazySingleTon<MeshLoader> {
 public:
  static void LoadWithAssimp(const std::string &file_path,
                             std::vector<VertexData> &out_vertex,
                             std::vector<unsigned int> &out_indices);

 private:
};

class ModelFinder {
 public:
  void UpdateNameAndID(const std::string &name, const uint64_t &id);

  uint64_t GetIdByName(const std::string &name);

  std::string GetNameById(const uint64_t &id);

  bool Erase(uint64_t id);

  bool Erase(const std::string &name);

 private:
  std::map<std::string, uint64_t> mModelNameToId;
  std::map<uint64_t, std::string> mModelIdToName;
};

class ModelLoader : public LazySingleTon<ModelLoader> {
 public:
  /// @brief Get the Id By Name object
  /// @param name the name u set for the model, or just use the file name
  /// @return Model UID
  uint64_t GetId(const std::string &name);

  /// @brief Get the Name By Id object
  /// @param id  the id u get from GetId
  /// @return  the name u set for the model, or just use the file name
  std::string GetName(const uint64_t &id);

  Handle GetVaoHandle(const uint64_t &id);

  Handle GetVaoHandle(const std::string &name);

  Handle GetVboHandle(const uint64_t &id);

  Handle GetVboHandle(const std::string &name);

  unsigned int GetIndicesCount(const uint64_t &id);
  unsigned int GetIndicesCount(const std::string &name);

 public:
  /// @brief Load model and upload to GPU instantly
  /// @param file_path the path of the model
  /// @param name  model name
  UID LoadModelAndUpLoad(const std::string &file_path,
                         const std::string &name = "");

  /// @brief Load model but not upload to GPU
  /// @param file_path the file path of the model
  /// @param name model name
  UID LoadModelButNotUpLoad(const std::string &file_path,
                            const std::string &name = "");

 private:
  void UpdateNameToId(const std::string &name, uint64_t &id);

  void UpdateFile(const std::string &file_path, uint64_t &id);

 private:
  static void UseFileNameAsName(const std::string &file_path,
                                std::string &name);

  // GPU working section
 private:
  bool UpLoadToGPU(uint64_t &id);

  bool CreateBuffer(uint64_t &id);

 private:
  struct DataHandles {
    Handle vao;
    Handle vbo;
  };
  struct DataBuffer {
    GBuffer gIndices;
    GBuffer gVertex;
  };

  std::map<uint64_t, std::string> mModelFilePath;
  std::map<uint64_t, MeshData> mMeshData;
  std::map<uint64_t, DataHandles> mDataHandles;
  std::map<uint64_t, DataBuffer> mGBuffers;

  ModelFinder finder;
  IDGenerator mIdGenerator;
};