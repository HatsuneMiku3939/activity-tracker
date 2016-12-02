#include "aps_meter.h"

namespace tracker {
namespace aps {

ActionPerSecondMeter::ActionPerSecondMeter() : action_per_second(0) {
}

ActionPerSecondMeter::~ActionPerSecondMeter() {
  // TODO stop Thread
}

int ActionPerSecondMeter::getActionPerSecond(void) {
  return action_per_second;
}

void ActionPerSecondMeter::start(void) {
  DWORD dwThreadId = NULL;
  CreateThread(NULL, 0, ActionPerSecondMeter::ThreadFunc, this, 0, &dwThreadId);
}

DWORD WINAPI ActionPerSecondMeter::ThreadFunc(PVOID pvParam) {
  ActionPerSecondMeter *self = (ActionPerSecondMeter *)pvParam;

  BYTE previous_key_status[256];
  BYTE current_key_status[256];

  memset(previous_key_status, 0, 256);
  memset(current_key_status, 0, 256);

  GetKeyState(0) ;
  GetKeyboardState(current_key_status);
  memcpy(current_key_status, previous_key_status, 256);

  int measure_counter = 0;
  int key_pressed = 0;
  while (true) {
    memset(current_key_status, 0, 256);

    GetKeyState(0) ;
    if (GetKeyboardState(current_key_status)) {
      for (int i = 0; i < 256; i++) {
        int key_down = current_key_status[i] >> 7;
        if (current_key_status[i] != previous_key_status[i] && key_down)
          key_pressed += 1;

        previous_key_status[i] = current_key_status[i];
      }
    }

    if (measure_counter == 50) {
      self->action_per_second = key_pressed;
      measure_counter = 0;
      key_pressed = 0;
    }
    Sleep(20);
    measure_counter += 1;
  }

  return 0;
}

} // ns aps
} // ns tracker
