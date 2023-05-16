#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

enum ShaderType {
  Vertex,
  Fragment,
  Geometry,
  TessellationControl,
  TessellationEvaluation,
  Compute,
  EndCount,
  Include,
  ShaderUnoknown
};
#define ENUM_TO_STR(value) \
  if (type == value) return #value;
static const char* ShaderTypeToStr(ShaderType type) {
  ENUM_TO_STR(Vertex)
  ENUM_TO_STR(Vertex)
  ENUM_TO_STR(Fragment)
  ENUM_TO_STR(Geometry)
  ENUM_TO_STR(TessellationControl)
  ENUM_TO_STR(TessellationEvaluation)
  ENUM_TO_STR(Compute)
  ENUM_TO_STR(EndCount)
  ENUM_TO_STR(Include)
  ENUM_TO_STR(ShaderUnoknown)
  return "";
}
#undef ENUM_TO_STR

class InternalDefine {
 public:
  InternalDefine() = default;
  ~InternalDefine() = default;
  std::string GetPreDefineByType(ShaderType type) const {
    return m_shaderPreDefine[type];
  }

 private:
  std::array<std::string, ShaderType::EndCount> m_shaderPreDefine = {
      "VERTEX_SHADER",       "FRAGMENT_SHADER",        "GEOMETRY_SHADER",
      "TESS_CONTROL_SHADER", "TESS_EVALUATION_SHADER", "COMPUTE_SHADER"};
};

class ShaderPreProcessor;

class ShaderFile {
  friend ShaderPreProcessor;

 public:
  ShaderFile() = default;
  explicit ShaderFile(std::string file_path)
      : m_filePath(std::move(file_path)) {
    GetShaderTypeByExtension();
    ReadFile();
  }
  virtual ~ShaderFile() {
    m_content.clear();
    m_filePath.clear();
    m_shaderType = ShaderUnoknown;
    m_shaderVersion = 0;
  }

 public:
  [[nodiscard]] const ShaderType& GetType() const { return m_shaderType; }
  [[nodiscard]] const int& GetVersion() const { return m_shaderVersion; }
  [[nodiscard]] const std::string& GetFilePath() const { return m_filePath; }
  std::vector<std::string>& Content() {
    // if (GetType() == ShaderUnoknown) {
    //   return std::vector<std::string>();
    // }
    if (m_content.empty()) ReadFile();
    return m_content;
  }

 private:
  /// @brief get shader type by extension
  void GetShaderTypeByExtension() {
    std::map<std::string, ShaderType> extensionToShaderType = {
        {".vert", ShaderType::Vertex},
        {".frag", ShaderType::Fragment},
        {".geom", ShaderType::Geometry},
        {".tesc", ShaderType::TessellationControl},
        {".tese", ShaderType::TessellationEvaluation},
        {".comp", ShaderType::Compute}};
    if (extensionToShaderType.find(
            std::filesystem::path(m_filePath).extension().string()) !=
        extensionToShaderType.end())
      m_shaderType = extensionToShaderType
          [std::filesystem::path(m_filePath).extension().string()];
  }
  /// @brief read file line by line and store in content
  void ReadFile() {
    std::ifstream file(m_filePath);
    if (!file.is_open()) {
      std::cout << "ShaderFile::ReadFile() : Failed to open file\n";
    }

    for (std::string line; std::getline(file, line);) m_content.push_back(line);
    ParseVersion();
    file.close();
  }
  /// @brief get version from the shader file
  void ParseVersion() {
    for (auto& line : m_content) {
      if (line.find("#version") != std::string::npos) {
        std::string version = line.substr(line.find("#version") + 8);
        m_shaderVersion = std::stoi(version);
        return;
      }
    }
  }

 protected:
  std::vector<std::string> m_content;  // original content line by line without
                                       // any processing
  std::string m_filePath{};
  ShaderType m_shaderType{ShaderUnoknown};
  int m_shaderVersion{0};
};

class ShaderInclude : public ShaderFile {
 public:
  ShaderInclude() = default;
  explicit ShaderInclude(std::string file_path) : ShaderFile(file_path) {
    m_shaderType = Include;
    PostProcess();
  }

  std::string ReplaceInclude(ShaderType type) {
    InternalDefine defines;
    std::string src =
        "// Include file: " + m_filePath + "\n";  // add file path as comment
    AddFileTypeDefine(src, type);  // Add default define for shader type
    for (auto& line : m_content) {
      src += line + "\n";
    }
    AddFileTypeUndef(src, type);
    src += "// End of include file: " + m_filePath + "\n";
    return src;
  }

 private:
  static void AddFileTypeDefine(std::string& line, ShaderType type) {
    line += "#define " + InternalDefine().GetPreDefineByType(type) + "\n";
  }
  static void AddFileTypeUndef(std::string& line, ShaderType type) {
    line += "#undef " + InternalDefine().GetPreDefineByType(type) + "\n";
  }

  void PostProcess() {
    for (int i = 0; i < m_content.size(); i++) {
      if (m_content[i].find("//@section-start") != std::string::npos) {
        std::cout << "Find section start. start to remove all string before "
                     "section\n";
        m_content.erase(m_content.begin(), m_content.begin() + i + 1);
        std::cout << "Removed " << i << " lines\n";
      } else if (m_content[i].find("//@section-end") != std::string::npos) {
        std::cout
            << "Find section end. start to remove all string after section\n ";
        m_content.erase(m_content.begin() + i, m_content.end());
      }
    }
  }
};

class ShaderPreProcessor {
 public:
  explicit ShaderPreProcessor(std::string dir_path)
      : m_directoryPath(dir_path) {
    LoadResources(m_directoryPath);
  }
  void Process() {
    for (auto& [file_path, shader] : m_shaderFiles) {
      std::cout << "Processing file: " << file_path << "\n";
      for (auto& line : shader.Content()) {
        if (line.find("#include") != std::string::npos) {
          std::string include_file_path =
              std::filesystem::path(file_path).parent_path().string() + "\\" +
              line.substr(line.find(_include_sys_prefix) + 1,
                          line.find(_include_sys_suffix) -
                              line.find(_include_sys_prefix) - 1);
          auto include = GetInclude(include_file_path);
          line = include.ReplaceInclude(shader.GetType());
        }
      }
    }
  }

  void SaveTo(std::string dir) {
    for (auto& [file_path, shader] : m_shaderFiles) {
      std::filesystem::path path(file_path);
      //      std::cout << path.parent_path().string() << std::endl;
      std::string fixed_path =
          dir + "\\" +
          path.parent_path().string().substr(m_directoryPath.size() + 1);
      std::filesystem::create_directories(fixed_path);
      fixed_path += "\\" + path.filename().string();
      std::ofstream file(fixed_path);
      for (auto& line : shader.Content()) {
        file << line << "\n";
      }
      file.close();
    }

    std::cout << "Save to " << dir << " done\n";
  }

 private:
  void LoadResources(std::string dir_path) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
      if (entry.is_directory()) {
        std::cout << "Find directory: " << entry.path().string() << std::endl;
        LoadResources(entry.path().string());
      }

      else {
        if (entry.path().extension() == ".vert" ||
            entry.path().extension() == ".frag" ||
            entry.path().extension() == ".geom" ||
            entry.path().extension() == ".tesc" ||
            entry.path().extension() == ".tese" ||
            entry.path().extension() == ".comp") {
          ShaderFile shader(entry.path().string());
          m_shaderFiles[entry.path().string()] = shader;
        } else if (entry.path().extension() == ".h" ||
                   entry.path().extension() == ".hpp") {
          ShaderInclude include(entry.path().string());
          m_shaderIncludes[entry.path().string()] = include;
        }
      }
    }
  }

 private:
  ShaderInclude GetInclude(std::string include) {
    if (m_shaderIncludes.find(include) != m_shaderIncludes.end())
      return m_shaderIncludes[include];
    else {
      ShaderInclude temp_file(include);
      if (temp_file.GetType() == Include) {
        m_shaderIncludes[include] = temp_file;
      }
      return temp_file;
    }
  }

 private:
  std::string m_directoryPath{};
  std::map<std::string, ShaderFile> m_shaderFiles;
  std::map<std::string, ShaderInclude> m_shaderIncludes;

  static constexpr auto _include_sys_prefix = "<";
  static constexpr auto _include_sys_suffix = ">";

  static constexpr auto _include_cus_prefix = "\"";
};