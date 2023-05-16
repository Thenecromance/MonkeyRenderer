#include <iostream>

#include "ShaderPreProcessor.hpp"

auto main(int argc, const char** argv) -> int {
  if (argc <= 2) {
    std::cout << "must be 2 arguments\n";
    return 1;
  }

  std::string originalPath = argv[1];
  std::string outputPath = argv[2];

  std::cout << originalPath << std::endl;
  std::cout << outputPath << std::endl;

  std::cout << "ShaderPreProcessor Initialize\n";
  ShaderPreProcessor processor(originalPath);
  processor.Process();
  processor.SaveTo(outputPath);
  return 0;
}
