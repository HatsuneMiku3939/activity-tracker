#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <vector>
#include <utility>

#include "json/json.h"

namespace tracker {
namespace logger {

class Log {
public:
  Log(std::string tag);
  virtual ~Log();

  virtual std::string to_json(void);
  std::string getTag(void);

  template<typename ValueType>
  std::pair<bool, ValueType> get_property(std::string name);

  template<>
  std::pair<bool, int> Log::get_property<int>(std::string name) {
    Json::Value property = root[name];
    if (property.type() == 0) return std::make_pair(false, 0);
    return std::make_pair(true, property.asInt());
  }

  template<>
  std::pair<bool, std::string> Log::get_property<std::string>(std::string name) {
    Json::Value property = root[name];
    if (property.type() == 0) return std::make_pair(false, "");
    return std::make_pair(true, property.asString());
  }

  template<>
  std::pair<bool, double> Log::get_property<double>(std::string name) {
    Json::Value property = root[name];
    if (property.type() == 0) return std::make_pair(false, 0.0);
    return std::make_pair(true, property.asDouble());
  }

  template<>
  std::pair<bool, bool> Log::get_property<bool>(std::string name) {
    Json::Value property = root[name];
    if (property.type() == 0) return std::make_pair(false, false);
    return std::make_pair(true, property.asBool());
  }

  template<typename ValueType>
  void add_property(std::string name, ValueType value) {
    root[name] = value;
  }

protected:
  Json::Value root;
};


class Filter {
public:
  virtual bool apply(Log *log) = 0;
};

// TODO: use smart pointer
class Output {
public:
  virtual void emit(const std::string log_json) = 0;

public:
  void receive(Log *log);
  Output &withFilter(Filter *filter);

private:
  std::vector<Filter *> filters;
};

class Logger {
public:
  Logger();
  virtual ~Logger();
  void send(Log &log);

public:
  void route(std::string tag, Output *output);

private:
  std::vector<std::pair<std::string, Output *>> routes;
};

} // ns logger
} // ns tracker

#endif
