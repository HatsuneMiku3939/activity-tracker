#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace tracker {
namespace utils {

std::wstring replaceAll(std::wstring str, const std::wstring &from, const std::wstring &to);
std::wstring escapeSpecialWord(std::wstring str);
std::string utf8_conv(std::wstring str);

} // ns utils
} // ns tracker

#endif
