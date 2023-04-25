//
// Created by Thenecromance on 2023/4/24.
//

#include "FileWatcherModule.hpp"

#include <cassert>
#include <functional>
#include <iostream>
#include <queue>

// using topological sort to check if there is a circle in the graph
bool FileWatcherModule::hasCircle() {
  std::map<UID, int> inDegree;
  {
    for (auto& [key, val] : mGraph) {
      inDegree[key] = 0;
    }
    for (auto& [key, val] : mGraph) {
      for (auto& child : val) {
        inDegree[child]++;
      }
    }
  }
  // topological sort
  {
    std::queue<UID> q;
    for (auto& [key, val] : inDegree) {
      if (val == 0) {
        q.push(key);
      }
    }
    while (!q.empty()) {
      auto cur = q.front();
      q.pop();
      for (auto& child : mGraph[cur]) {
        inDegree[child]--;
        if (inDegree[child] == 0) {
          q.push(child);
        }
      }
    }
  }

  for (auto& [key, val] : inDegree) {
    if (val != 0) {
      return true;
    }
  }
  return false;
}

void FileWatcherModule::AddFile(std::string file) {
  if (mFiles.has(file)) {
    return;
  }
  auto FileId = mIDGenerator.next_id(File);
  mFiles.emplace(file, FileId);

  mGraph[FileId] = {};

  mFileLastModified[file] = getFileLastModified(file);

  if (hasCircle()) {
    assert(false && "found circle in it !");
  }
}

void FileWatcherModule::AddShader(std::string shader, Handle handle) {
  // if shader handle is 0,means this shader is not loaded
  if (handle == 0) {
    return;
  }
  // now the handle contain it's type
  handle = wrapHandle(handle, Shader);
  // if shader container has this shader,return
  if (mHandles.has(handle)) {
    return;
  }
  auto ShaderId = mIDGenerator.next_id(Shader);
  if (!mFiles.has(shader)) {
    AddFile(shader);
  }
  auto FileId = mFiles.at(shader);

  // add shader info
  mHandles.emplace(handle, ShaderId);

  mGraph[FileId].push_back(ShaderId);  // add edge
  mGraph[ShaderId] = {};               // add node to graph

  if (hasCircle()) {
    assert(false && "found circle in it !");
  }
}

void FileWatcherModule::AddProgram(Handle program,
                                   std::vector<Handle> shaders) {
  // if program handle is 0,means this program is not loaded
  if (program == 0) {
    return;
  }
  // if program container has this program,return
  if (mHandles.has(program)) {
    return;
  }
  program = wrapHandle(program, Program);
  auto ProgramId = mIDGenerator.next_id(Program);
  // add program info
  mHandles.emplace(program, ProgramId);
  mGraph[ProgramId] = {};  // add node to graph
  for (auto shader : shaders) {
    shader = wrapHandle(shader, Shader);
    if (!mHandles.has(shader)) {
      continue;
    }
    auto ShaderId = mHandles.at(shader);
    mGraph[ShaderId].push_back(ProgramId);  // add edge
  }

  if (hasCircle()) {
    assert(false && "found circle in it !");
  }
}
void FileWatcherModule::AddFileToFile(std::string file, std::string parent) {
  if (!mFiles.has(file)) {
    AddFile(file);
  }
  if (!mFiles.has(parent)) {
    AddFile(parent);
  }
  auto FileId = mFiles.at(file);
  auto ParentId = mFiles.at(parent);
  mGraph[ParentId].push_back(FileId);

  if (hasCircle()) {
    assert(false && "found circle in it !");
  }
}

void FileWatcherModule::OnUpdate() {
  for (auto it : mFiles.getMap()) {
    auto file = it.first;
    auto FileId = it.second;
    auto lastModified = getFileLastModified(file);

    // file has been modified
    if ((lastModified - mFileLastModified[file]).count() > 300020) {
      mFileLastModified.at(file) = lastModified;
      auto fileId = mFiles.at(file);

      std::vector<UID> modifiedEffects;
      std::function<void(UID)> collect_all_effects = [&](UID ids) -> void {
        for (auto cur : mGraph[ids]) {
          modifiedEffects.push_back(cur);
          collect_all_effects(cur);
        }
      };
      collect_all_effects(fileId);
      // here is the all effects need to update
      for (auto effectId : modifiedEffects) {
        std::cout << file << " changed: " << mHandles[effectId]
                  << ":RealHandle " << GetRealHandle(effectId)
                  << " need to update" << std::endl;
      }
    }
  }
}
std::filesystem::file_time_type FileWatcherModule::getFileLastModified(
    std::string file) {
  return std::filesystem::last_write_time(file);
}
// int FileWatcherModule::GetHandleType(UID id) {
//   // get handle type
//   auto handle = mHandles[id];
//   return static_cast<int>((handle >> RECORD_TYPE_BIT_SHIFT) & MAX_TYPE_ID);
// }
Handle FileWatcherModule::GetRealHandle(UID id) {
  // get real handle
  auto handle = mHandles[id];
  return handle >> RECORD_TYPE_BIT_SHIFT;
}
