
#include <vector>

#include "CommonDef.hpp"

class Buffer;

class WorkerInterface {
 public:
  virtual ~WorkerInterface() = default;

  virtual Handle Compile(std::vector<char> &buffer) = 0;

  virtual Handle Compile(Buffer &buffer) = 0;

  virtual bool Release(Handle handle) = 0;

  virtual bool Reload(Handle handle) = 0;

  virtual bool IsValid(Handle handle) = 0;
};
