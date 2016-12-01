#include <string>
#include <iostream>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <Winsock2.h>
#include <windows.h>

#include "utils_win.h"
#include "logger.h"

using namespace tracker::utils;
using namespace tracker::logger;

class ConslogOutput : public Output {
  void emit(std::wstring log_json) {
    std::wcout << log_json << std::endl;
  }
};

// TODO, error logging
class FluentdUdpOutput : public Output {
public:
  FluentdUdpOutput(): port(20039), host("FLUENTD_HOST_IP_IS_HERE") {
    if (!initWinsock()) {
      throw std::runtime_error("FluentdUdpOutput: Winsock initialize error");
    }
  }

  void emit(std::wstring log_json) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    send(utf8_conv.to_bytes(log_json));
  }

private:
  bool initWinsock(void) {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR;
  }

  void send(std::string &log) {
    SOCKET sendSocket = INVALID_SOCKET;
    sockaddr_in recvAddr;

    sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == INVALID_SOCKET) {
      // std::cout << "create UDP socket error" << std::endl;
      return;
    }

    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(port);
    recvAddr.sin_addr.s_addr = inet_addr(host.c_str());

    if (sendto(sendSocket, log.c_str(), log.size(), 0, (SOCKADDR *)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
      // std::cout << "UDP sendto error" << std::endl;
    }

    closesocket(sendSocket);
  }

  int port;
  std::string host;
};

class ActivityLog : public Log {
public:
  ActivityLog(std::wstring &stat,
    std::wstring &activeUsername,
    std::wstring &activeWindowText,
    std::wstring &activeFilename) : status(stat), username(activeUsername),
                                    windowText(activeWindowText), filename(activeFilename), Log(L"activity") {
  }

  std::wstring to_json(void) {
    return std::wstring(L"{")
           + L" \"status\":\"" + status + L"\""
           + L",\"user\":\"" + username + L"\""
           + L",\"window_text\":\"" + windowText + L"\""
           + L",\"filename\":\"" + filename
           + L"\"}\n";
  }

private:
  std::wstring status;
  std::wstring username;
  std::wstring windowText;
  std::wstring filename;
};

class InternalLog : public Log {
public:
  InternalLog(std::wstring lev, std::wstring msg): Log(L"log"), level(lev), message(msg) {
  }

  std::wstring to_json(void) {
    return std::wstring(L"{")
           + L" \"timestamp\":\"" + current_time_and_date()
           + L",\"level\":\"" + level
           + L",\"message\":\"" + message
           + L"\"}\n";
  }

private:
  std::wstring current_time_and_date() {
    std::wstringstream ss;

    std::time_t t = std::time(NULL);
    ss << std::put_time(std::localtime(&t), L"%c");

    return ss.str();
  }

  std::wstring level;
  std::wstring message;
};

int main(void) {
  Logger logger;

  FluentdUdpOutput fluentd_udp_output;
  logger.route(L"activity", &fluentd_udp_output);

  ConslogOutput consolg_output;
  logger.route(L"log", &consolg_output);
  int counter = 0;

  logger.send(InternalLog(L"INFO", L"starting activity-tracker"));
  while(true) {
    HWND activeWindow = GetForegroundWindow();
    DWORD activePID;
    GetWindowThreadProcessId(activeWindow, &activePID);

    std::wstring activeWindowText = getActiveWindowText(activeWindow);
    std::wstring activeFilename = getAcitveFilename(activePID);
    std::wstring activeUsername = getActiveUsername();
    std::wstring status = isIdle() ? L"idle" : L"active";

    logger.send(ActivityLog(status, activeUsername, activeWindowText, activeFilename));
    if (counter == 60) {
      logger.send(InternalLog(L"INFO", L"activity-tracker is running..."));
      counter = 0;
    }

    counter++;
    Sleep(1000);
  }
}

