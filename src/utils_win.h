#ifndef __UTILS_WIN_H__
#define __UTILS_WIN_H__

#include <string>
#include <windows.h>

namespace tracker {
namespace utils {

bool isIdle(void);
std::wstring getAcitveFilename(DWORD activePID);
std::wstring getActiveWindowText(HWND activeHWND);
std::wstring getActiveUsername(void);

} // ns utils
} // ns tracker

#endif
