#ifndef __APS_METER_H__
#define __APS_METER_H__

#include <windows.h>

namespace tracker {
namespace aps {

class ActionPerSecondMeter {
public:
  ActionPerSecondMeter();
  virtual ~ActionPerSecondMeter();

  int getActionPerSecond(void);
  void start(void);

private:
  int action_per_second;
  static DWORD WINAPI ThreadFunc(PVOID pvParam);
};

} // ns aps
} // ns tracker

#endif
