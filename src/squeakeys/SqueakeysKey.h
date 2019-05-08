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

typedef PluginKey<key_type_id> SqueakeysKey;

constexpr
bool isSqueakeysKey(Key key) {
  return SqueakeysKey::verifyType(key);
}

}  // namespace squeakeys
}  // namespace kaleidoglyph
