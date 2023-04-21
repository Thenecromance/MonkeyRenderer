#pragma once

#include "Marcos/Marcos.hpp"
#include "WorkerInterface.hpp"

class Buffer;

ROOT_BGN()

class ShaderWorker : public WorkerInterface {
 public:
  ShaderWorker() = default;

  ~ShaderWorker() override = default;

  Handle Compile(std::vector<char> &buffer) override;

  Handle Compile(Buffer &buffer) override;

  bool Release(Handle handle) override;

  bool Reload(Handle handle) override;

  bool IsValid(Handle handle) override;

 private:
  // check shader compile success or not
  static bool CheckCompileStatus(Handle handle);
};

class ProgramWorker : public WorkerInterface {
 public:
  ProgramWorker() = default;

  ~ProgramWorker() override = default;

  Handle Compile(std::vector<char> &buffer) override;

  Handle Compile(Buffer &buffer) override;

  bool Release(Handle handle) override;

  bool Reload(Handle handle) override;

  bool IsValid(Handle handle) override;

 private:
  static bool CheckLinkStatus(Handle handle);
};

ROOT_END()