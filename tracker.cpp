#include <string>
#include <iostream>
#include <codecvt>
#include <locale>

#include <Winsock2.h>
#include <windows.h>

#include "utils_win.h"

#define FLUENTD_UDP_PORT  (20039)
#define FLUENTD_HOST      ("FLUENTD_HOST_IP_IS_HERE")

bool initWinsock(void);
void sendToFluentdUDP(std::string &status);

std::wstring genStatusJSON(std::wstring &status,
    std::wstring &activeUsername,
    std::wstring &activeWindowText,
    std::wstring &activeFilename) {

  std::wstring json = std::wstring(L"{")
    + L" \"status\":\"" + status + L"\""
    + L",\"user\":\"" + activeUsername + L"\""
    + L",\"window_text\":\"" + activeWindowText + L"\""
    + L",\"filename\":\"" + activeFilename
    + L"\"}\n";

  return json;
}

int main(void) {
  using namespace tracker::utils;

  if (!initWinsock()) {
    std::cout << "initWinsock error" << std::endl;
  }

  std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  while(true) {
    std::wstring status;

    HWND activeWindow = GetForegroundWindow();
    DWORD activePID;
    GetWindowThreadProcessId(activeWindow, &activePID);

    std::wstring activeWindowText = getActiveWindowText(activeWindow);
    std::wstring activeFilename = getAcitveFilename(activePID);
    std::wstring activeUsername = getActiveUsername();

    if (isIdle()) {
      status = genStatusJSON(std::wstring(L"idle"),   activeUsername, activeWindowText, activeFilename);
    } else {
      status = genStatusJSON(std::wstring(L"active"), activeUsername, activeWindowText, activeFilename);
    }
    sendToFluentdUDP(utf8_conv.to_bytes(status));
    Sleep(1000);
  }
}

// -----------------------------------------------------------------------
bool initWinsock(void) {
  WSADATA wsaData;
  return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
}

void sendToFluentdUDP(std::string &status) {
  SOCKET sendSocket = INVALID_SOCKET;
  sockaddr_in recvAddr;

  sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sendSocket == INVALID_SOCKET) {
    std::cout << "create UDP socket error" << std::endl;
    return;
  }

  recvAddr.sin_family = AF_INET;
  recvAddr.sin_port = htons(FLUENTD_UDP_PORT);
  recvAddr.sin_addr.s_addr = inet_addr(FLUENTD_HOST);

  if (sendto(sendSocket, status.c_str(), status.size(), 0, (SOCKADDR *)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
    std::cout << "UDP sendto error" << std::endl;
  }

  closesocket(sendSocket);
}

