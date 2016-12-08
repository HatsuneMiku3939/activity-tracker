#ifndef __UTILS_WIN_H__
#define __UTILS_WIN_H__

#include <string>
#include <windows.h>

namespace tracker {
namespace utils {

bool isIdle(void);
std::string getAcitveFilename(DWORD activePID);
std::string getActiveWindowText(HWND activeHWND);
std::string getActiveUsername(void);

} // ns utils
} // ns tracker

#endif
