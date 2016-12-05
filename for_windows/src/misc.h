#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <Winsock2.h>
#include <windows.h>
#include <codecvt>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <list>

#include "../../src/utils_win.h"
#include "../../src/logger/logger.h"
#include "../../src/aps_meter.h"

using namespace tracker::logger;

class EditBoxOutput : public Output {
private:
  HWND hEditBox;
  std::list<std::wstring> loglist;
  int count;
public:
  EditBoxOutput(HWND _hEditBox): hEditBox(_hEditBox), count(0) {}
  void emit(std::wstring log_json) {
    if (loglist.size() > 100){
      loglist.pop_front();
    }
    loglist.push_back(log_json);
    ::SendMessage(hEditBox, EM_REPLACESEL,0,(LPARAM)log_json.c_str());
    count++;

    if (count > 300) {
      count  == 0;
      std::wstring log = L"";
      for (std::list<std::wstring>::iterator it = loglist.begin(); it != loglist.end(); ++it) {
        log += (*it) + L"\r\n";
      }
      ::SendMessage(hEditBox, WM_SETTEXT,0,(LPARAM)log.c_str());
    }
  }
};

// TODO, error logging
class FluentdUdpOutput : public Output {
public:
  FluentdUdpOutput(): port(20039), host("FLUENTD_ADDRESS") {
    if (!initWinsock()) {
      throw std::runtime_error("FluentdUdpOutput: Winsock initialize error");
    }
  }

  void emit(std::wstring log_json) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    send(utf8_conv.to_bytes(log_json + L"\n"));
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
    std::wstring &activeFilename,
    int actionPerSecond) : status(stat), username(activeUsername),
                           windowText(activeWindowText), filename(activeFilename),
                           aps(actionPerSecond), Log(L"activity") {
  }

  std::wstring to_json(void) {
    return std::wstring(L"{")
           + L" \"status\":\"" + status + L"\""
           + L",\"user\":\"" + username + L"\""
           + L",\"window_text\":\"" + windowText + L"\""
           + L",\"filename\":\"" + filename + L"\""
           + L",\"action_per_second\":" + std::to_wstring(aps)
           + L" }";
  }

private:
  std::wstring status;
  std::wstring username;
  std::wstring windowText;
  std::wstring filename;
  int aps;
};

class InternalLog : public Log {
public:
  InternalLog(std::wstring lev, std::wstring msg): Log(L"log"), level(lev), message(msg) { }

  std::wstring to_json(void) {
    return std::wstring(L"{")
           + L" \"timestamp\":\"" + current_time_and_date() + L"\""
           + L",\"level\":\"" + level + L"\""
           + L",\"message\":\"" + message
           + L"\"}";
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

class CounterFilter : public Filter {
public:
  CounterFilter(int cnt = 60) : max_count(cnt), count(0) { }

  bool apply(Log *log) {
    count += 1;

    if (count >= max_count) {
      count = 0;
      return true;
    }

    return false;
  }

  void set_max(int cnt) {
    count = 0;
    max_count = cnt;
  }

private:
  int count;
  int max_count;
};
