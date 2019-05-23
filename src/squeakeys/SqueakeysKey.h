// -*- c++ -*-

#pragma once

#include <Arduino.h>

#include <assert.h>
#include "kaleidoglyph/Key.h"
#include "kaleidoglyph/Key/PluginKey.h"

#if defined(SQUEAKEYS_CONSTANTS_H)
#include SQUEAKEYS_CONSTANTS_H
#else
namespace kaleidoglyph {
namespace squeakeys {

constexpr byte key_type_id{0b0000100};

}  // namespace squeakeys
}  // namespace kaleidoglyph
#endif

namespace kaleidoglyph {
namespace squeakeys {

//typedef PluginKey<key_type_id> SqueakeysKey;

enum class SqueakeysKeyVariant : byte {
  button,
  speed,   // modifier
  move,
  scroll,
  jump,    // relative
  warp,    // absolute
};

enum class SqueakeysDirection : byte {
  up,
  down,
  left,
  right,
};

enum class SqueakeysSpeed : byte {
  slow,
  normal,
  fast,
};

constexpr byte direction_up    {1 << 0};
constexpr byte direction_down  {1 << 1};
constexpr byte direction_left  {1 << 2};
constexpr byte direction_right {1 << 3};

constexpr byte button_left   {1 << 0};
constexpr byte button_right  {1 << 1};
constexpr byte button_middle {1 << 2};
constexpr byte button_prev   {1 << 3};
constexpr byte button_next   {1 << 4};

constexpr uint8_t variant_bits{3};
constexpr uint8_t variant_mask_offset = 8 - variant_bits;
constexpr byte value_mask = 0xFF >> variant_bits;
constexpr byte variant_mask = ~value_mask;

class SqueakeysKey : public PluginKey<key_type_id> {
 public:
  constexpr
  SqueakeysKey(byte value) : PluginKey<key_type_id>(value) {}

  SqueakeysKey(Key key) : PluginKey<key_type_id>(key) {}

  SqueakeysKeyVariant variant() const {
    return SqueakeysKeyVariant((data() & variant_mask) >> variant_mask_offset);
  }
  byte value() const {
    return data() & value_mask;
  }
};

constexpr
bool isSqueakeysKey(Key key) {
  return SqueakeysKey::verifyType(key);
}

constexpr
SqueakeysKey squeakeysMovementKey(byte direction) {
  byte data = static_cast<byte>(SqueakeysKeyVariant::move);
  data <<= variant_mask_offset;
  data |= direction;
  return SqueakeysKey(data);
}

constexpr
SqueakeysKey squeakeysScrollKey(byte direction) {
  byte data = static_cast<byte>(SqueakeysKeyVariant::scroll);
  data <<= variant_mask_offset;
  data |= direction;
  return SqueakeysKey(data);
}

constexpr
SqueakeysKey squeakeysButtonKey(byte buttons) {
  byte data = static_cast<byte>(SqueakeysKeyVariant::button);
  data <<= variant_mask_offset;
  data |= buttons;
  return SqueakeysKey(data);
}

constexpr
SqueakeysKey squeakeysWarpKey(byte direction) {
  byte data = static_cast<byte>(SqueakeysKeyVariant::warp);
  data <<= variant_mask_offset;
  data |= direction;
  return SqueakeysKey(data);
}

constexpr
SqueakeysKey squeakeysSpeedKey(byte speed) {
  byte data = static_cast<byte>(SqueakeysKeyVariant::speed);
  data <<= variant_mask_offset;
  data |= speed;
  return SqueakeysKey(data);
}

}  // namespace squeakeys

constexpr squeakeys::SqueakeysKey Key_MouseButtonLeft =
    squeakeys::squeakeysButtonKey(squeakeys::button_left);
constexpr squeakeys::SqueakeysKey Key_MouseButtonRight =
    squeakeys::squeakeysButtonKey(squeakeys::button_right);
constexpr squeakeys::SqueakeysKey Key_MouseButtonMiddle =
    squeakeys::squeakeysButtonKey(squeakeys::button_middle);
constexpr squeakeys::SqueakeysKey Key_MouseButtonPrev =
    squeakeys::squeakeysButtonKey(squeakeys::button_prev);
constexpr squeakeys::SqueakeysKey Key_MouseButtonNext =
    squeakeys::squeakeysButtonKey(squeakeys::button_next);

constexpr squeakeys::SqueakeysKey Key_MouseUp =
    squeakeys::squeakeysMovementKey(squeakeys::direction_up);
constexpr squeakeys::SqueakeysKey Key_MouseDown =
    squeakeys::squeakeysMovementKey(squeakeys::direction_down);
constexpr squeakeys::SqueakeysKey Key_MouseLeft =
    squeakeys::squeakeysMovementKey(squeakeys::direction_left);
constexpr squeakeys::SqueakeysKey Key_MouseRight =
    squeakeys::squeakeysMovementKey(squeakeys::direction_right);

constexpr squeakeys::SqueakeysKey Key_MouseWarpEnd =
    squeakeys::squeakeysWarpKey(0);
constexpr squeakeys::SqueakeysKey Key_MouseWarpNW =
    squeakeys::squeakeysWarpKey(squeakeys::direction_up | squeakeys::direction_left);
constexpr squeakeys::SqueakeysKey Key_MouseWarpNE =
    squeakeys::squeakeysWarpKey(squeakeys::direction_up | squeakeys::direction_right);
constexpr squeakeys::SqueakeysKey Key_MouseWarpSW =
    squeakeys::squeakeysWarpKey(squeakeys::direction_down | squeakeys::direction_left);
constexpr squeakeys::SqueakeysKey Key_MouseWarpSE =
    squeakeys::squeakeysWarpKey(squeakeys::direction_down | squeakeys::direction_right);

}  // namespace kaleidoglyph
