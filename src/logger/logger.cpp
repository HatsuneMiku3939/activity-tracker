#include "logger.h"

namespace tracker {
namespace logger {

Log::Log(std::string tag) {
  root["tag"] = tag;
}

Log::~Log() {
}

std::string Log::getTag(void) {
  return root["tag"].asString();
}

std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

std::string Log::to_json(void) {
  Json::Value tag_removed = root;
  tag_removed.removeMember("tag");

  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "";
  std::string json = Json::writeString(builder, tag_removed);

  return replaceAll(json, "\n", " ");
}

Output &Output::withFilter(Filter *filter) {
  filters.push_back(filter);

  return *this;
}

void Output::receive(Log *log) {
  for (auto filter = filters.begin(); filter != filters.end(); filter++) {
    if (!(*filter)->apply(log)) {
      return;
    }
  }

  emit(log->to_json());
}

Logger::Logger() {
}

Logger::~Logger() {
}

void Logger::route(std::string tag, Output *output) {
  routes.push_back(make_pair(tag, output));
}

void Logger::send(Log &log) {
  for (auto route = routes.begin(); route != routes.end(); route++) {
    std::string tag = (*route).first;
    Output *output = (*route).second;

    if (tag == log.getTag()) {
      output->receive(&log);
    }
  }
}

} // ns logger
} // ns tracker
