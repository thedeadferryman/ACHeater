// Copyright 2024 Karl 'Charon' Meinkopf
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <inttypes.h>
#include "config.h"
#include "macro.h"

enum class DeviceScreen : uint8_t {
    ROOM_TEMP,
    TARGET_TEMP,
    DELTA,
    HYSTER,
    BRIGHT,
};

namespace detail {
#define DECL_NEXT_SCREEN(_Cur, _Next) case DeviceScreen::_Cur: return (DeviceScreen::_Next)

    FORCEINLINE constexpr auto nextScreen(DeviceScreen current) {
        switch (current) {
            DECL_NEXT_SCREEN(ROOM_TEMP, DELTA);
            DECL_NEXT_SCREEN(DELTA, TARGET_TEMP);
            DECL_NEXT_SCREEN(TARGET_TEMP, HYSTER);
            DECL_NEXT_SCREEN(HYSTER, BRIGHT);
            DECL_NEXT_SCREEN(BRIGHT, ROOM_TEMP);
        }
    }

#undef DECL_NEXT_SCREEN
}

struct DeviceState {
    DeviceScreen screen;

    uint8_t displayBrightness;

    double hysterDia;
    double setTemp;

    FORCEINLINE_NOSTATIC void nextScreen() {
        screen = detail::nextScreen(screen);
    }
};

constexpr const auto DefaultDeviceState = DeviceState{
        .screen = DeviceScreen::ROOM_TEMP,
        .displayBrightness = DEFAULT_DISP_BRIGHTNESS,
        .hysterDia = DEFAULT_HYSTER_DIA,
        .setTemp = DEFAULT_SET_TEMP,
};