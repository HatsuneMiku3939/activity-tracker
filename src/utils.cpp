#include "utils.h"
#include <codecvt>
#include <locale>

namespace tracker {
namespace utils {

std::wstring replaceAll(std::wstring str, const std::wstring &from, const std::wstring &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

std::wstring escapeSpecialWord(std::wstring str) {
  str = replaceAll(str, L"\\", L"\\\\");
  str = replaceAll(str, L"\"", L"\\\"");

  return str;
}

std::string utf8_conv(std::wstring str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  return utf8_conv.to_bytes(str);
}

} // ns utils
} // ns tracker
