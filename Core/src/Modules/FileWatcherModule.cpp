//
// Created by Thenecromance on 2023/4/24.
//

#include "FileWatcherModule.hpp"

#include <glad/glad.h>

#include <cassert>
#include <functional>
#include <iostream>
#include <queue>

#include "File.hpp"
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
  UID FileId = mIDGenerator.next_id(eFile);
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
  handle = wrapHandle(handle, eShader);
  // if shader container has this shader,return
  if (mHandles.has(handle)) {
    return;
  }
  UID ShaderId = mIDGenerator.next_id(eShader);
  if (!mFiles.has(shader)) {
    AddFile(shader);
  }
  UID FileId = mFiles.at(shader);

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
  program = wrapHandle(program, eProgram);
  UID ProgramId = mIDGenerator.next_id(eProgram);
  // add program info
  mHandles.emplace(program, ProgramId);
  mGraph[ProgramId] = {};  // add node to graph
  for (auto shader : shaders) {
    shader = wrapHandle(shader, eShader);
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

std::filesystem::file_time_type FileWatcherModule::getFileLastModified(
    const std::string& file) {
  return std::filesystem::last_write_time(file);
}

int FileWatcherModule::GetHandleType(UID id) {
  // get handle type
  auto handle = mHandles[id];
  return static_cast<int>((handle >> RECORD_TYPE_BIT_SHIFT) & MAX_TYPE_ID);
}
Handle FileWatcherModule::GetRealHandle(UID id) {
  // get real handle
  auto handle = mHandles[id];
  return handle & MAX_TYPE_ID;
}

FileWatcherModule::WrappedHandle FileWatcherModule::wrapHandle(
    Handle handle, RecordType type) {
  //  return (handle << RECORD_TYPE_BIT_SHIFT) | type;
  return (type << RECORD_TYPE_BIT_SHIFT) | type;
}

void ReCompileShader(Handle shader, std::string file) {
  File shd(file);
  std::string src;
  shd.read(src);
  auto s = src.c_str();
  glShaderSource(shader, 1, &s, nullptr);
  glCompileShader(shader);
}

void ReLinkProgram(Handle program) { glLinkProgram(program); }

void FileWatcherModule::OnUpdate() {
  for (const auto& it : mFiles.getMap()) {
    auto file = it.first;
    auto FileId = it.second;
    auto lastModified = getFileLastModified(file);

    // file has been modified
    if ((lastModified - mFileLastModified[file]).count() > 5 * 10000) {
      mFileLastModified.at(file) = lastModified;

      // catch all handles which get effect from this file
    }
  }
}
