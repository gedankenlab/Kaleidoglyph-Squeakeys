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
  // If it's not a SqueakeysKey, ignore it.
  if (! isSqueakeysKey(event.key)) {
    warp_status_ = 0;
    return EventHandlerResult::proceed;
  }

  SqueakeysKey squeakeys_key{event.key};

  SqueakeysKeyVariant key_type = squeakeys_key.variant();

  if (key_type == SqueakeysKeyVariant::warp) {
    if (event.state.toggledOn()) {
      byte warp_direction = squeakeys_key.value();
      if (warp_direction == 0) {
        warp_status_ = 0;
        return EventHandlerResult::proceed;
      }
      if (warp_status_ == 0) {
        warp_position_ = {warp_center, warp_center};
      }
      Serial.println(warp_status_, BIN);
      byte warp_level_x = warp_status_ & 0x0F;
      byte warp_level_y = warp_status_ >> 4;
      if (warp_direction & direction_up) {
        ++warp_level_y;
        warp_position_.y -= warp_center >> warp_level_y;
      }
      if (warp_direction & direction_down) {
        ++warp_level_y;
        warp_position_.y += warp_center >> warp_level_y;
      }
      if (warp_direction & direction_left) {
        ++warp_level_x;
        warp_position_.x -= warp_center >> warp_level_x;
      }
      if (warp_direction & direction_right) {
        ++warp_level_x;
        warp_position_.x += warp_center >> warp_level_x;
      }
      warp_status_ = warp_level_y;
      Serial.println(warp_status_, BIN);
      warp_status_ <<= 4;
      Serial.println(warp_status_, BIN);
      warp_level_x &= 0b00001111;
      Serial.println(warp_level_x, BIN);
      warp_status_ |= warp_level_x;
      Serial.println(warp_status_, BIN);
      //warp_status_ = (warp_level_y << 4) | (warp_level_x & 0x0F);

      hid::mouse::absolute::Report absolute_mouse_report;
      absolute_mouse_report.moveCursorTo(warp_position_.x, warp_position_.y);
      // There should be no need to press mouse buttons here.
      //absolute_mouse_report.pressButtons(buttons_);
      controller_.sendMouseReport(absolute_mouse_report);
    }
    return EventHandlerResult::proceed; // proceed?
  } else {
    warp_status_ = 0;
  }

  switch (squeakeys_key.variant()) {

    case SqueakeysKeyVariant::button: {
      if (event.state.toggledOn()) {
        buttons_ |= squeakeys_key.value();
      } else {
        // TODO: check for other mouse button keys held first
        buttons_ &= ~(squeakeys_key.value());
      }
      hid::mouse::Report mouse_report;
      mouse_report.pressButtons(buttons_);
      controller_.sendMouseReport(mouse_report);
      break;
    }

    case SqueakeysKeyVariant::move: {
      if (event.state.toggledOn()) {
        if (move_direction_ == 0) {
          move_speed_ = 3;
          last_move_update_time_ = Controller::scanStartTime();
          move_start_time_ = Controller::scanStartTime();
        }
        move_direction_ |= squeakeys_key.value();
      } else {
        // TODO: check for other mouse move keys held first
        move_direction_ &= ~(squeakeys_key.value());
        if (move_direction_ == 0) {
          move_speed_ = 0;
        }
      }
      break;
    }

    default:
      break;
  }

  // If it's a key press, we don't need to search the active_keys matrix for any
  // other keys; we can just add it to the Squeakeys status cache(s). What we do
  // next depends on the type of key.

  // If we get a movement key press, just add it to the status bits. If the
  // previous status was all zeros, reset speed & acceleration (or maybe that's
  // done on release).

  // If it's a mouse button key, first check if that button was already on. If
  // so, release it and send a mouse report. Then, (regardless of whether it was
  // added before) set that button's status bit and trigger a mouse report.

  // Mouse wheel movement might be different than mouse pointer movement, but
  // probably just insofar as v & h are definitely independent.

  // Mouse warping should reset like Kaleidoscope does.

  // Mouse jumping should trigger only on key press, with releases
  // ignored. Warping should work the same.

  if (event.state.toggledOn()) {
  } else {
    // It's a key release. Here, for some keys, we need to search the active
    // keys to make sure no other keys of the same value are still held before
    // clearing the corresponding bit in the status variable.
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

  int8_t x_delta{0};
  int8_t y_delta{0};

  if (move_direction_ & direction_up) {
    y_delta -= delta;
  }
  if (move_direction_ & direction_down) {
    y_delta += delta;
  }
  if (move_direction_ & direction_left) {
    x_delta -= delta;
  }
  if (move_direction_ & direction_right) {
    x_delta += delta;
  }




  // uint16_t total_speed = scaled_speed << move_speed_;
  // total_speed += remainder;
  // remainder = total_speed;
  // remainder >>= move_speed_;
  // int8_t speed = total_speed >> 8;

  

  hid::mouse::Report mouse_report;
  mouse_report.moveCursor(x_delta, y_delta);
  mouse_report.pressButtons(buttons_);
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
