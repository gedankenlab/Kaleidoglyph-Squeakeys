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

  SqueakeysHandler(Controller& controller) : controller_(controller) {}

  void preKeyswitchScan();
  EventHandlerResult onKeyEvent(KeyEvent& event);

 private:
  Controller & controller_;
  // These are useful for tracking pressed mouse keys. Probably not necessary.
  Bitfield<total_keys> active_squeakeys_addrs_;
  bool any_active_squeakeys{false};

  // Updates need to happen much more often than every 256ms, so eight bits
  // should be sufficient for timestamps.
  uint16_t move_start_time_;
  uint8_t wheel_start_time_;
  uint8_t last_move_update_time_{0};

  // State variables for mouse movement.
  byte move_direction_{0};
  int8_t move_speed_{0};
  uint16_t move_speed_limit_{move_speed_limit_normal};

  // State variables for wheel.
  uint16_t scroll_speed_{0};
  uint16_t scroll_speed_limit{scroll_speed_limit_normal};
};

} // namespace qukeys {
} // namespace kaleidoglyph {
