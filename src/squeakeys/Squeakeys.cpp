// -*- c++ -*-

#include "squeakeys/Squeakeys.h"

#include <Arduino.h>

#include <kaleidoglyph/Key.h>
#include <kaleidoglyph/KeyAddr.h>
#include <kaleidoglyph/KeyEvent.h>
#include <kaleidoglyph/KeyState.h>
#include <kaleidoglyph/Plugin.h>
#include <kaleidoglyph/Controller.h>
#include <kaleidoglyph/hid/mouse.h>

namespace kaleidoglyph {
namespace squeakeys {

// Event handler
EventHandlerResult SqueakeysHandler::onKeyEvent(KeyEvent& event) {
  if (! isSqueakeysKey(event.key))
    return EventHandlerResult::proceed;

  if (event.state.toggledOn()) {
    move_speed_ = 3;
    last_move_update_time_ = Controller::scanStartTime();
    move_start_time_ = Controller::scanStartTime();
  } else {
    move_speed_ = 0;
  }
  return EventHandlerResult::proceed;
}

uint8_t scaledSpeed(uint8_t elapsed_time) {
  if (elapsed_time < 128) {
    uint16_t t2 = elapsed_time * elapsed_time;
    return 1 + (t2 >> 7);
  } else {
    uint16_t remaining_time = 256 - elapsed_time;
    uint16_t t2 = remaining_time * remaining_time;
    return 255 - (t2 >> 7);
  }
}

void SqueakeysHandler::preKeyswitchScan() {
  // If there are any mouse movement or scroll keys held, we need to act
  // here. Perhaps the best thing would be to search the controller's active
  // keys array and return the held keys. It's probably worth it to use a
  // bitfield to track which keys are active mouse keys if we do this.
  // if (any_active_squeakeys) {
  //   for (KeyAddr k : active_squeakeys_addrs_) {
  //   }
  // }
  if (move_speed_ == 0)
    return;
  uint16_t current_time = Controller::scanStartTime();
  uint16_t elapsed_time = current_time - last_move_update_time_;
  if (elapsed_time < move_update_interval)
    return;

  last_move_update_time_ += move_update_interval;

  uint16_t scaled_speed;
  uint16_t elapsed_acceleration_time = current_time - move_start_time_;
  uint16_t scaled_time = elapsed_acceleration_time >> 6;
  if (scaled_time < 255) {
    scaled_speed = scaledSpeed(scaled_time);
  } else {
    scaled_speed = 255;
  }
  // if (elapsed_acceleration_time < move_acceleration_duration) {
  //   uint8_t scaled_time = elapsed_acceleration_time >> 6;
  //   // uint8_t scaled_time =
  //   //     uint32_t(elapsed_acceleration_time * 256) / move_acceleration_duration;
  //   scaled_speed = scaledSpeed(scaled_time);
  // } else {
  //   scaled_speed = 255;
  // }
  static uint8_t remainder{0};
  scaled_speed += remainder;
  remainder = scaled_speed & (0xFF >> 4);

  int8_t delta = scaled_speed >> 4;


  // uint16_t total_speed = scaled_speed << move_speed_;
  // total_speed += remainder;
  // remainder = total_speed;
  // remainder >>= move_speed_;
  // int8_t speed = total_speed >> 8;

  

  hid::mouse::Report mouse_report;
  mouse_report.moveCursor(delta, 0);
  controller_.sendMouseReport(mouse_report);
}

// int8_t scaledSpeed(uint16_t elapsed_time) {
//   uint16_t t_end = 256;
//   uint16_t v_end = 256;
//   if (elapsed_time > (t_end / 2)) {
//     elapsed_time = t_end - elapsed_time;
//   }
//   uint16_t t2 = elapsed_time * elapsed_time;
//   uint16_t v = t2 / (t_end / 2) * (v_end / t_end);
//   if (elapsed_time > (t_end / 2)) {
//     return v_end - v;
//   } else {
//     return v;
//   }
// }

uint8_t linearSpeed(bool x, bool y, uint16_t speed) {
  if (x && y) {
    // sqrt(2)
    return (speed * 70) / 99;
  } else {
    return speed;
  }
}

} // namespace squeakeys {
} // namespace kaleidoglyph {
