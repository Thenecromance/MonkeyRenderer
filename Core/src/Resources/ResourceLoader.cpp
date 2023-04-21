#include "ResourceLoader.hpp"

#include "Buffer/Buffer.hpp"
#include "Logger.hpp"
#include "SnowFlake.hpp"
#include "WorkerInterface.hpp"

UID ResourceLoader::LoadAndCompile(const std::string &path,
                                   const std::string &name) {
  Logger::get("ResourceLoader")->trace("Loading file: {}", path);
  File f(path);
  //	UID uid = f.Hash();
  Snowflake snowflake;
  UID uid = snowflake.next_id();
  if (!name.empty()) {
    mapUIDName[uid] = name;
  } else {
    auto ext = f.getExtension().substr(1);

    ext[0] = toupper(ext[0]);
    mapUIDName[uid] = f.getFileNameWithoutExtension() + ext;
  }

  mapPath[uid] = f;

  // if IWorker is nullptr , ResourceLoader will be just a file manager
  if (pWorker != nullptr) {
    std::vector<char> file_data;

    if (!f.read(file_data)) {
      Logger::get("ResourceLoader")->error("Failed to read file: {}", path);
      return 0;
    }
    auto buffer = InsertBufferInfo(path, file_data);
    Handle handle = pWorker->Compile(buffer);
    mapUIDMapper[uid] = handle;
  }
  return uid;
}

UID ResourceLoader::LoadNotCompile(const std::string &path,
                                   const std::string &name) {
  File f(path);
  UID uid = f.Hash();

  if (!name.empty()) {
    mapUIDName[uid] = name;
  } else {
    mapUIDName[uid] =
        f.getFileNameWithoutExtension() + f.getExtension().substr(1);
  }

  mapPath[uid] = f;

  return uid;
}

bool ResourceLoader::Erase(UID uid) { return EraseByUID(uid); }

bool ResourceLoader::EraseByUID(UID uid) {
  if (mapUIDName.find(uid) != mapUIDName.end()) {
    mapUIDName.erase(uid);
  }

  if (mapPath.find(uid) != mapPath.end()) {
    mapPath.erase(uid);
  }

  if (mapUIDMapper.find(uid) != mapUIDMapper.end()) {
    mapUIDMapper.erase(uid);
  }

  return true;
}

bool ResourceLoader::EraseByName(const std::string &name) {
  for (auto &it : mapUIDName) {
    if (it.second == name) {
      return EraseByUID(it.first);
    }
  }

  return false;
}

bool ResourceLoader::EraseByPath(const std::string &path) {
  for (auto &it : mapPath) {
    if (it.second.getRelativePath() == path) {
      return EraseByUID(it.first);
    }
  }

  return false;
}

bool ResourceLoader::IsCompiled(UID uid) {
  return mapUIDMapper.find(uid) != mapUIDMapper.end();
}

UID ResourceLoader::GetUID(const std::string &name) {
  for (auto &it : mapUIDName) {
    if (it.second == name) {
      return it.first;
    }
  }

  return 0;
}

UID ResourceLoader::GetUID(const File &file) { return file.Hash(); }

std::string ResourceLoader::GetResourceName(UID uid) {
  if (mapUIDName.find(uid) != mapUIDName.end()) {
    return mapUIDName[uid];
  }

  return "Invalid UID";
}

bool ResourceLoader::CleanUpAll() {
  int iReleasedCount = 0;
  if (pWorker != nullptr) {
    for (auto &[key, val] : mapUIDMapper) {
      if (pWorker->Release(val)) iReleasedCount++;
    }
  }
  mapUIDName.clear();
  mapPath.clear();
  mapUIDMapper.clear();
  Logger::get("ResourceLoader")->info("Released {} resources", iReleasedCount);
  return true;
}

Buffer ResourceLoader::InsertBufferInfo(const std::string &path,
                                        std::vector<char> &buffer) {
  return Buffer(buffer);
}

Handle ResourceLoader::GetHandle(UID uid) {
  if (mapUIDMapper.find(uid) != mapUIDMapper.end()) {
    return mapUIDMapper[uid];
  }

  return 0;
}


