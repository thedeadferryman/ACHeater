#pragma once

#include <Arduino.h>

// region Features
constexpr const auto DEBUG_ENABLED = false;
constexpr const auto OVERHEAT_SEN_ENABLED = false;
// endregion

// region Hardware
constexpr const auto ADC_RESOLUTION = 10u;

constexpr const auto P_DISP_DIO = 4;
constexpr const auto P_DISP_CLK = 3;

constexpr const auto P_ENC_S1 = 7;
constexpr const auto P_ENC_S2 = 6;
constexpr const auto P_ENC_BTN = 5;

constexpr const auto P_HEATER = 12;

constexpr const auto P_OVERHEAT_SEN = A2;
// endregion

// region Values
constexpr const auto ROOM_TEMP_QUERY_RATE = 100; // ms
constexpr const auto OVERHEAT_QUERY_RATE = 100; // ms
constexpr const auto SETTINGS_SAVE_DELAY = 5000; // ms

constexpr const auto OVERHEAT_SEN_R = 10000;
constexpr const auto OVERHEAT_SEN_B = 3950;
constexpr const auto OVERHEAT_SEN_t = 25;
constexpr const auto OVERHEAT_SEN_Rt = 10000;

constexpr const auto OVERHEAT_THRESHOLD = 65.;


constexpr const auto MIN_SET_TEMP = 12.;
constexpr const auto DEFAULT_SET_TEMP = 25.;
constexpr const auto MAX_SET_TEMP = 55.;

constexpr const auto MIN_HYSTER_DIA = 0.;
constexpr const auto DEFAULT_HYSTER_DIA = 0.2;
constexpr const auto MAX_HYSTER_DIA = 5.;

constexpr const uint8_t MIN_DISP_BRIGHTNESS = 0u;
constexpr const uint8_t DEFAULT_DISP_BRIGHTNESS = 5u;
constexpr const uint8_t MAX_DISP_BRIGHTNESS = 7u;
// endregion
