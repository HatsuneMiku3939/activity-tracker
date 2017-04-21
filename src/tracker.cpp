#include <string>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <Winsock2.h>
#include <windows.h>

#include "utils_win.h"
#include "logger.h"
#include "aps_meter.h"

using namespace tracker::logger;

class ConslogOutput : public Output {
  void emit(std::string log_json) {
    std::cout << log_json << std::endl;
  }
};

// TODO, error logging
class FluentdUdpOutput : public Output {
public:
  FluentdUdpOutput(std::string fluentd_host, int fluentd_port): port(fluentd_port), host(fluentd_host) {
    if (!initWinsock()) {
      throw std::runtime_error("FluentdUdpOutput: Winsock initialize error");
    }
  }

  void emit(std::string log_json) {
    send(log_json + "\n");
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
  ActivityLog(std::string &status,
              std::string &session_id,
              std::string &username,
              std::string &windowText,
              std::string &filename,
              int aps) : Log("activity") {

    add_property<std::string>("status", status);
    add_property<std::string>("session_id", session_id);
    add_property<std::string>("user", username);
    add_property<std::string>("window_text", windowText);
    add_property<std::string>("filename", filename);
    add_property<int>("action_per_second", aps);
  }
};

class InternalLog : public Log {
public:
  InternalLog(std::string level, std::string message): Log("log") {
    add_property<std::string>("timestamp", current_time_and_date());
    add_property<std::string>("level", level);
    add_property<std::string>("message", message);
  }

private:
  std::string current_time_and_date() {
    std::stringstream ss;

    std::time_t t = std::time(NULL);
    ss << std::put_time(std::localtime(&t), "%c");

    return ss.str();
  }
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

void usage(void) {
  std::cout <<
            "activity-tracker\n"
            "\n"
            "An agent of personal activity monitoring system for Windows desktop.\n"
            "\n"
            "USAGE: activity-tracker.exe FLUENTD_HOST_IP FLUENTD_UDP_PORT\n"
            "       activity-tracker.exe 192.168.11.5 20039\n"
            << std::endl;
}

int main(int argc, char **argv) {
  using namespace tracker::utils;
  using namespace tracker::aps;

  if (argc != 3) {
    usage();
    return 0;
  }

  std::string fluentd_host = argv[1];
  int fluentd_port = std::stoi(argv[2]);

  Logger logger;
  FluentdUdpOutput fluentd_udp_output(fluentd_host, fluentd_port);
  logger.route("activity", &fluentd_udp_output);

  ConslogOutput consolg_output;
  CounterFilter counter_filter(0);
  consolg_output.withFilter(&counter_filter);
  logger.route("log", &consolg_output);

  logger.send(InternalLog("INFO", "starting activity-tracker"));
  counter_filter.set_max(60);

#ifdef DEBUG
  logger.route("activity", &consolg_output);
  counter_filter.set_max(0);
#endif

  ActionPerSecondMeter aps;
  aps.start();

  std::string activeWindowText;
  std::string activeFilename;
  std::string activeUsername;
  std::string status;
  int actionPerSecond;
  std::string session_id;

  // pre-assign status for detect status change
  status = isIdle() ? "idle" : "active";

  // generate initial sesison id
  session_id = getUUID();

  while (true) {
    DWORD tick = GetTickCount();

    HWND activeWindow = GetForegroundWindow();
    DWORD activePID;
    GetWindowThreadProcessId(activeWindow, &activePID);

    activeWindowText = getActiveWindowText(activeWindow);
    activeFilename = getAcitveFilename(activePID);
    activeUsername = getActiveUsername();
    std::string current_status = isIdle() ? "idle" : "active";
    actionPerSecond = aps.getActionPerSecond();

    // if status change from idle to active
    if (status == "idle" && current_status == "active") {
      // generate a new sesison id
      session_id = getUUID();
    }
    status = current_status;

    logger.send(ActivityLog(status, session_id,
                            activeUsername, activeWindowText, activeFilename, actionPerSecond));
    logger.send(InternalLog("INFO", "activity-tracker is running..."));

    // wait 1000ms per loop
    Sleep(1000 - (GetTickCount() - tick));
  }
}

