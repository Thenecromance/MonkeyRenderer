#pragma once

#include <array>
#include <string>
class File;

class ShaderOperator {
 public:
  explicit ShaderOperator(std::string file_path);
  ~ShaderOperator();
  std::string ReadSource(std::array<std::string, 10>& includePaths);
  // if use ShaderOperator, the include path will be relative to the file
  std::array<std::string, 10> GetIncludesArray();

 private:
  bool IsInclude(const std::string& line);
  bool IsExplain(const std::string& line);
  bool IsVersionDefine(const std::string& line);
  std::string RemoveGroupedExplain(const std::string& line);
  /// @brief change the #include <path> to the relative path to make the include
  /// file merge to the source file
  /// @param line #include <path>
  /// @return the relative path of the include file
  std::string GetIncludePath(const std::string& line);

  std::string GeneratePreDefine();
  std::string InsertPreDefine();

 private:
  File* pFile{};

 private:
  static constexpr auto _inc_prefix = "#include";
  static constexpr auto _sys_prefix = "<";
  static constexpr auto _sys_suffix = ">";
  static constexpr auto _user_prefix = "\"";
  //  static constexpr auto _user_suffix = "\"";

  static constexpr auto _version_prefix = "#version";
  //   static constexpr auto _version_suffix = "\n";

  static constexpr auto _explain_prefix = "//";
  static constexpr auto _explain_suffix = "\n";

  // this mark is a little bit different from the above
  static constexpr auto _grouped_explain_prefix = "/*";
  static constexpr auto _grouped_explain_suffix = "*/";

  static constexpr auto _include_path_start = "//@section-start";
  std::array<std::string, 10> _includePaths{};
};