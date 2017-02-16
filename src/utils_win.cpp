#include "utils.h"
#include "utils_win.h"

#include <windows.h>
#include <tlhelp32.h>
#include <Rpc.h>

namespace tracker {
namespace utils {

bool isIdle(void) {
  LASTINPUTINFO li;

  li.cbSize = sizeof(LASTINPUTINFO);
  ::GetLastInputInfo(&li);

  DWORD te = ::GetTickCount();
  double elapsed = (te - li.dwTime) / 1000.0;

  return elapsed >= 30.0;
}

std::string getAcitveFilename(DWORD activePID) {
  HANDLE processSnap;
  PROCESSENTRY32W pe32;

  processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (processSnap == INVALID_HANDLE_VALUE) {
    return std::string("NULL");
  }

  pe32.dwSize = sizeof(PROCESSENTRY32W);
  if (!Process32FirstW(processSnap, &pe32)) {
    CloseHandle(processSnap);
    return std::string("NULL");
  }

  do {
    if (pe32.th32ProcessID == activePID) {
      std::wstring activeFilename(pe32.szExeFile);
      CloseHandle(processSnap);
      return utf8_conv(escapeSpecialWord(activeFilename));
    }
  } while (Process32NextW(processSnap, &pe32));

  CloseHandle(processSnap);
  return std::string("NULL");
}

std::string getActiveWindowText(HWND activeHWND) {
  wchar_t buf[1024];
  if (GetWindowTextW(activeHWND, buf, 1024)) {
    std::wstring activeWindowText(buf);
    return utf8_conv(escapeSpecialWord(activeWindowText));
  }

  return std::string("NULL");
}

std::string getActiveUsername(void) {
  wchar_t username[1024];
  DWORD len = 1024;
  if (GetUserNameW(username, &len)) {
    return utf8_conv(escapeSpecialWord(std::wstring(username)));
  }

  return std::string("NULL");
}

std::string getUUID(void) {
  UUID uuid;
  std::string uuid_string;

  if (UuidCreate(&uuid) == RPC_S_OK) {
    wchar_t *uuid_buffer;
    UuidToStringW(&uuid, (RPC_WSTR *)(&uuid_buffer));

    uuid_string = utf8_conv(uuid_buffer);
    RpcStringFreeW((RPC_WSTR *)(&uuid_buffer));
  } else {
    uuid_string = std::string("NULL");
  }

  return uuid_string;
}

} // ns utils
} // ns tracker
