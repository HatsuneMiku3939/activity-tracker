#include "utils.h"

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

} // ns utils
} // ns tracker
