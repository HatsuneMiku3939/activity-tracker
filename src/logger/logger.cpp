#include "logger.h"

namespace tracker {
namespace logger {

Log::Log(std::string tag) {
  this->tag = tag;
}

Log::~Log() {
}

std::string Log::getTag(void) {
  return tag;
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
