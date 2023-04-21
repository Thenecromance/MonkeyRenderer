#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(Grid)
class GridModule {
 public:
  explicit GridModule(world &ecs);
};
MOD_END(Grid)