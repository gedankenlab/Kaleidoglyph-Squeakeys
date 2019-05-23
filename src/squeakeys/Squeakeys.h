// -*- c++ -*-

#include <Arduino.h>

#include <kaleidoglyph/KeyAddr.h>
#include <kaleidoglyph/Key.h>
#include <kaleidoglyph/Plugin.h>
#include <kaleidoglyph/Keymap.h>
#include <kaleidoglyph/Controller.h>
#include <kaleidoglyph/cKey.h>
#include <kaleidoglyph/EventHandlerResult.h>
#include <kaleidoglyph/hid/keyboard.h>

#include "squeakeys/SqueakeysKey.h"

namespace kaleidoglyph {
namespace squeakeys {

class SqueakeysHandler : public EventHandler {

 public:
  static constexpr uint8_t wheel_update_interval{50};
  static constexpr uint8_t move_update_interval{1};
  static constexpr uint16_t move_speed_limit_normal{5000};
  static constexpr uint16_t move_speed_limit_fast{10000};
  static constexpr uint16_t move_speed_limit_slow{2000};
  static constexpr uint16_t time_to_get_to_full_speed{500};
  static constexpr uint16_t scroll_speed_limit_normal{5000};
  static constexpr uint16_t scroll_speed_limit_fast{10000};
  static constexpr uint16_t scroll_speed_limit_slow{2000};

  static constexpr uint16_t move_acceleration_duration{2000};

  static constexpr byte direction_up    {1 << 0};
  static constexpr byte direction_down  {1 << 1};
  static constexpr byte direction_left  {1 << 2};
  static constexpr byte direction_right {1 << 3};

  static constexpr uint16_t warp_width{32767};
  static constexpr uint16_t warp_height{32767};
  static constexpr uint16_t warp_center {1 << 14};

  SqueakeysHandler(Controller& controller) : controller_(controller) {}

  void preKeyswitchScan();
  EventHandlerResult onKeyEvent(KeyEvent& event);

 private:
  Controller & controller_;
  // These are useful for tracking pressed mouse keys. Probably not
  // necessary. I'm thinking of storing state internally, and searching the
  // active_keys array only when necessary (on release of certain types). Maybe
  // it will still be useful to track the number of mouse movement keys held to
  // short-circuit that check in most cases.
  Bitfield<total_keys> active_squeakeys_addrs_;
  bool any_active_squeakeys{false};
  byte active_squeakeys_count_{0};

  // Updates need to happen much more often than every 256ms, so eight bits
  // should be sufficient for timestamps.
  uint16_t move_start_time_;
  uint8_t wheel_start_time_;
  uint8_t last_move_update_time_{0};

  // One bitfield should be sufficient for scrolling and movement, but maybe
  // that doesn't transfer efficiently to the HID report.
  byte move_and_scroll_direction_{0};
  
  // State variables for mouse movement.
  byte move_direction_{0};
  int8_t move_speed_{0};
  uint16_t move_speed_limit_{move_speed_limit_normal};

  // State variables for wheel.
  uint16_t scroll_speed_{0};
  uint16_t scroll_speed_limit{scroll_speed_limit_normal};
  // Scroll acceleration should be much slower than move acceleration.

  // State variable for buttons.
  byte buttons_{0};

  // warping
  byte warp_status_{0};
  struct {
    uint16_t x;
    uint16_t y;
  } warp_position_;
};

} // namespace qukeys {
} // namespace kaleidoglyph {
