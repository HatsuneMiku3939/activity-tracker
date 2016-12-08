#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <vector>

namespace tracker {
namespace logger {

class Log {
public:
  Log(std::string tag);
  virtual ~Log();

  virtual std::string to_json(void) = 0;
  std::string getTag(void);

protected:
  std::string tag;
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
