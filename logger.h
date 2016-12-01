#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <vector>

namespace tracker {
namespace logger {

class Log {
public:
  Log(std::wstring tag);
  virtual ~Log();

  virtual std::wstring to_json(void) = 0;
  std::wstring getTag(void);

protected:
  std::wstring tag;
};

class Filter {
public:
  virtual bool apply(Log *log) = 0;
};

// TODO: use smart pointer
class Output {
public:
  virtual void emit(const std::wstring log_json) = 0;

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
  void route(std::wstring tag, Output *output);

private:
  std::vector<std::pair<std::wstring, Output *>> routes;
};

} // ns logger
} // ns tracker

#endif
