
#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "SnowFlake.hpp"
typedef unsigned int Handle;
typedef long long UID;

/*
 just based on the snowflake and it's easy to handle each values' bit size.
 this module can easily include the file, shader, program, texture, etc. and can
 easy to check the uid is file or shader or program, etc.
 */

enum RecordType {
  File,
  Shader,
  Program,
  MaxTypeSize = (1L << 10) - 1  // just because of the snowflake, worker id has
                                // 5 bit so the max is 31
};

/// @brief simple wrapper for std::map to make it easier to use and make it can
/// be use key and value search in O(1)
/// @tparam Type target type
template <typename Type>
class Relation {
 public:
  void erase(UID& keyVal) {
    mIDToType.erase(keyVal);
    mTypeToID.erase(mIDToType[keyVal]);
  }
  void erase(Type& keyVal) {
    mTypeToID.erase(keyVal);
    mIDToType.erase(mTypeToID[keyVal]);
  }

  UID& at(Type& keyVal) { return mTypeToID.at(keyVal); }
  Type& at(UID& keyVal) { return mIDToType.at(keyVal); }
  UID& at(Type&& keyVal) { return mTypeToID.at(keyVal); }
  Type& at(UID&& keyVal) { return mIDToType.at(keyVal); }

  void emplace(Type&& keyVal, UID&& value) {
    mTypeToID.emplace(keyVal, value);
    mIDToType.emplace(value, keyVal);
  }

  void emplace(Type& keyVal, UID& value) {
    mTypeToID.emplace(keyVal, value);
    mIDToType.emplace(value, keyVal);
  }

  bool has(UID& keyVal) { return mIDToType.find(keyVal) != mIDToType.end(); }
  bool has(Type& keyVal) { return mTypeToID.find(keyVal) != mTypeToID.end(); }
  bool has(UID&& keyVal) { return mIDToType.find(keyVal) != mIDToType.end(); }
  bool has(Type&& keyVal) { return mTypeToID.find(keyVal) != mTypeToID.end(); }
  std::map<Type, UID>& getMap() { return mTypeToID; }

 public:
  Type& operator[](const UID& KeyVal) { return mIDToType[KeyVal]; }
  UID& operator[](const Type& KeyVal) { return mTypeToID[KeyVal]; }

  Type& operator[](UID&& Right) { return mIDToType[Right]; }
  UID& operator[](Type&& Right) { return mTypeToID[Right]; }

 private:
  std::map<Type, UID> mTypeToID;
  std::map<UID, Type> mIDToType;
};

/// @brief file watcher module is mainly for watching file change and update
class FileWatcherModule {
  static constexpr int RECORD_TYPE_BIT_SHIFT = 10;
  static constexpr int MAX_TYPE_ID = (1L << RECORD_TYPE_BIT_SHIFT) - 1;
  typedef Handle WrappedHandle;

 public:
  /// @brief if all file relations has circle,return true. file should not
  /// appera the circle relation
  /// @return  bool
  bool hasCircle();

  void AddFile(std::string file);
  void AddShader(std::string shader, Handle handle);
  void AddProgram(Handle program, std::vector<Handle> shaders);
  void AddFileToFile(std::string file, std::string parent);

  void OnUpdate();

 private:
//  int GetHandleType(UID id);
  Handle GetRealHandle(UID id);

 private:
  static WrappedHandle wrapHandle(Handle handle, RecordType type) {
    return (handle << RECORD_TYPE_BIT_SHIFT) | type;
  }

  std::filesystem::file_time_type getFileLastModified(std::string file);

 private:
  Relation<std::string> mFiles;      // maps of file path and uid
  Relation<WrappedHandle> mHandles;  // maps of handle and uid
  // file last time modified
  std::map<std::string, std::filesystem::file_time_type> mFileLastModified;

 private:
  std::map<UID, std::vector<UID>>
      mGraph;  // DAG graph key:parent value:childrens

  IDGenerator mIDGenerator;  // for create the unique id
};
