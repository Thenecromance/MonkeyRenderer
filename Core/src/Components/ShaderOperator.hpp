#pragma once

#include <array>
#include <string>
class File;
class ShaderOperator {
 public:
  explicit ShaderOperator(std::string file_path);
  std::string ReadSource(std::array<std::string, 10>& includePaths);
  // if use ShaderOperator, the include path will be relative to the file
  std::array<std::string, 10> GetIncludesArray();

 private:
  bool IsInclude(const std::string& line);
  bool IsExplain(const std::string& line);
  bool IsVersionDefine(const std::string& line);
  std::string RemoveGroupedExplain(const std::string& line);
  std::string GetIncludePath(const std::string& line);

  std::string GeneratePreDefine();

 private:
  File* pFile{};

 private:
  static constexpr auto _inc_prefix = "#include";
  static constexpr auto _sys_prefix = "<";
  static constexpr auto _sys_suffix = ">";
  static constexpr auto _user_prefix = "\"";
  static constexpr auto _user_suffix = "\"";

  static constexpr auto _version_prefix = "#version";
  //   static constexpr auto _version_suffix = "\n";

  static constexpr auto _explain_prefix = "//";
  static constexpr auto _explain_suffix = "\n";

  // this mark is a little bit different from the above
  static constexpr auto _grouped_explain_prefix = "/*";
  static constexpr auto _grouped_explain_suffix = "*/";

  std::array<std::string, 10> _includePaths{};  // it doesn't work why?
};