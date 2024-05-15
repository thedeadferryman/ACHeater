#pragma once

#include <inttypes.h>

constexpr const uint8_t INT_SLOW_CONF_RATE = 1u;
constexpr const uint8_t INT_FAST_CONF_RATE = 5u;

constexpr const auto DOUBLE_SLOW_CONF_RATE = 0.1;
constexpr const auto DOUBLE_FAST_CONF_RATE = 1.;

FORCEINLINE auto doubleEncModifier(int8_t dir, bool useFast) {
    return dir * (useFast ? DOUBLE_FAST_CONF_RATE : DOUBLE_SLOW_CONF_RATE);
}

FORCEINLINE auto doubleEncModifier(int8_t dir) {
    return doubleEncModifier(dir, true);
}

FORCEINLINE auto intEncModifier(int8_t dir, bool useFast) {
    return dir * (useFast ? INT_FAST_CONF_RATE : INT_SLOW_CONF_RATE);
}

FORCEINLINE auto intEncModifier(int8_t dir) {
    return intEncModifier(dir, false);
}