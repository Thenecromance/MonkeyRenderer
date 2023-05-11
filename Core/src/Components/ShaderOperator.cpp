#include "ShaderOperator.hpp"

#include <vector>

#include "File.hpp"
#include "Logger.hpp"

ShaderOperator::ShaderOperator(std::string file_path)
    : pFile(new File(file_path)) {}

bool ShaderOperator::IsInclude(const std::string& line) {
  if (IsExplain(line)) {
    return false;
  }
  return line.find(_inc_prefix) != std::string::npos;
}

bool ShaderOperator::IsExplain(const std::string& line) {
  auto start_loc = line.find(_explain_prefix);  // try to find "//"
  if (start_loc == std::string::npos) return false;

  // check if the "//" is in the middle of the line
  for (int i = 0; i < start_loc; i++) {
    if (line[i] == ' ') continue;
    if (line[i] == '\t') continue;
    return false;
  }
  return true;
}

bool ShaderOperator::IsVersionDefine(const std::string& line) {
  if (IsExplain(line)) return false;
  return line.find(_version_prefix) != std::string::npos;
}

std::string ShaderOperator::RemoveGroupedExplain(const std::string& line) {
  // remove the substring
  // TODO: remove the grouped explain

  return line;
}

std::string ShaderOperator::GetIncludePath(const std::string& line) {
  size_t start_loc = std::string::npos;
  size_t end_loc = std::string::npos;
  if (line.find(_sys_prefix) != std::string::npos) {
    start_loc = line.find(_sys_prefix);
    end_loc = line.find(_sys_suffix);
  } else {
    start_loc = line.find(_user_prefix);
    end_loc = line.find(_user_prefix, start_loc + 1);
   
  };

  if (start_loc == std::string::npos) {
    return "";
  }
  
  return pFile->getDirectory() + "\\" +
         line.substr(start_loc + 1, end_loc - start_loc - 1);
}

std::string ShaderOperator::ReadSource(
    std::array<std::string, 10>& includePaths) {
  if (!pFile->exists()) return "";
  std::vector<std::string> content;
  if (!pFile->read(content)) {
    return "";
  }
  // // insert the define for some functions which is used in the shader and
  // might
  // // caused the error
  // content.insert(content.begin(), GeneratePreDefine());
  // now parse the content line by line
  int include_idx = 0;

  for (auto& line : content) {
    if (IsVersionDefine(line)) {
      // clang-format off
      // if put the define to the first line in shader, compiler will raise
      // error C0204: version directive must be first statement and may not be repeated
      // so just put the define after the version define
      // clang-format on

      line += "\n" + GeneratePreDefine();
    }
    if (IsInclude(line)) {
      // read the include file and replace the include line with the include
      // file content
      // in this way, system could not watch the loop ref. this might be a core
      // problem

      auto includePath = GetIncludePath(line);
      Logger::get<ShaderOperator>()->info(
          "{} Load {}", pFile->getRelativePath(), includePath);
      if (includePath.empty()) {
        line = "";
        continue;
      }

      includePaths[include_idx++] = includePath;
      ShaderOperator includeShader(includePath);
      line = includeShader.ReadSource(includePaths);
      continue;
    }
    // cleanup the explain line
    if (IsExplain(line)) {
      line.clear();
      continue;
    }

    // other code is omitted
  }

  std::string finalSource;
  for (auto& line : content) finalSource += line + "\n";  // combine the content
  return RemoveGroupedExplain(finalSource);
}
std::string ShaderOperator::GeneratePreDefine() {
  if (pFile->getExtension() == ".vs" || pFile->getExtension() == ".vert") {
    return "#define VERTEX_SHADER\n";
  }
  if (pFile->getExtension() == ".fs" || pFile->getExtension() == ".frag") {
    return "#define FRAGMENT_SHADER\n";
  }
  if (pFile->getExtension() == ".gs" || pFile->getExtension() == ".geom") {
    return "#define GEOMETRY_SHADER\n";
  }
  if (pFile->getExtension() == ".cs" || pFile->getExtension() == ".comp") {
    return "#define COMPUTE_SHADER\n";
  }
  if (pFile->getExtension() == ".ts" || pFile->getExtension() == ".tesc") {
    return "#define TESS_CONTROL_SHADER\n";
  }
  if (pFile->getExtension() == ".ts" || pFile->getExtension() == ".tese") {
    return "#define TESS_EVALUATION_SHADER\n";
  }
  return "";
}
std::array<std::string, 10> ShaderOperator::GetIncludesArray() {
  return _includePaths;
}
