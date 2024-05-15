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

#include "config.h"

#include <EEPROM.h>

#include <GyverBME280.h>
#include <GyverNTC.h>

#include <GyverSegment.h>
#include <EncButton.h>

#include <TimerMs.h>
#include <GyverIO.h>

#include "DeviceState.h"
#include "macro.h"
#include "EncUtils.h"

#include "DisplayUtils.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

Disp1637_4 disp(P_DISP_DIO, P_DISP_CLK);
EncButtonT<P_ENC_S1, P_ENC_S2, P_ENC_BTN> enc;

GyverBME280 tempSen;
GyverNTC overheatSen;

TimerMs roomTempQueryTimer;
TimerMs overheatQueryTimer;
TimerMs saveSettingsTimer;

auto state = DefaultDeviceState;
auto overheating = false;

template<typename T, typename M>
FORCEINLINE constexpr auto applyModifier(
        T value, M modifier, T lower, T upper
) {
    return constrain(
            (value + modifier),
            lower, upper
    );
}

FORCEINLINE void initTimers() {
    roomTempQueryTimer.setPeriodMode();
    roomTempQueryTimer.setTime(ROOM_TEMP_QUERY_RATE);
    roomTempQueryTimer.start();

    if constexpr (OVERHEAT_SEN_ENABLED) {
        overheatQueryTimer.setPeriodMode();
        overheatQueryTimer.setTime(OVERHEAT_QUERY_RATE);
        overheatQueryTimer.start();
    }

    saveSettingsTimer.setTimerMode();
    saveSettingsTimer.setTime(SETTINGS_SAVE_DELAY);
}

FORCEINLINE void loadSettings() {
    if (enc.pressing()) {
        disp.setCursor(0);
        disp.println("rst");
        disp.update();

        EEPROM.put(0, 255);
    }

    if (EEPROM.read(0) != 255) {
        EEPROM.get(1, state);
    }
}

FORCEINLINE void saveSettings() {
    EEPROM.put(0, 1);
    EEPROM.put(1, state);
}

FORCEINLINE void processEncoderTurn() {
    if (enc.turn()) {
        auto dir = enc.dir();
        bool pressed = enc.pressing();

        switch (state.screen) {
            case DeviceScreen::ROOM_TEMP:
            case DeviceScreen::DELTA:
                break;
            case DeviceScreen::TARGET_TEMP:
                state.setTemp = applyModifier(
                        state.setTemp,
                        doubleEncModifier(dir, pressed),
                        MIN_SET_TEMP, MAX_SET_TEMP
                );
                saveSettingsTimer.start();
                break;
            case DeviceScreen::HYSTER:
                state.hysterDia = applyModifier(
                        state.hysterDia,
                        doubleEncModifier(dir, pressed),
                        MIN_HYSTER_DIA, MAX_HYSTER_DIA
                );
                saveSettingsTimer.start();
                break;
            case DeviceScreen::BRIGHT:
                state.displayBrightness = applyModifier(
                        state.displayBrightness,
                        intEncModifier(dir),
                        MIN_DISP_BRIGHTNESS, MAX_DISP_BRIGHTNESS
                );
                saveSettingsTimer.start();
                break;
        }
    }
}

FORCEINLINE void renderCurrentScreen(double currentTemp) {
    if (overheating) {
        disp.setCursor(0);
        disp.println("ovh");
        return;
    }

    switch (state.screen) {
        case DeviceScreen::ROOM_TEMP:
            displayValue(disp, 'c', currentTemp);
            break;
        case DeviceScreen::DELTA:
            displayValue(disp, 'd', currentTemp - state.setTemp);
            break;
        case DeviceScreen::HYSTER:
            displayValue(disp, 'h', state.hysterDia);
            break;
        case DeviceScreen::TARGET_TEMP:
            displayValue(disp, 't', state.setTemp);
        case DeviceScreen::BRIGHT:
            displayValue(disp, 'b', state.displayBrightness);
    }
}

void setup() {
    DEBUG {
        Serial.begin(9600);
    }

    if (!tempSen.begin()) {
        disp.setCursor(0);
        disp.println("-sen");
        disp.update();

        while (true);
    }

    if constexpr (OVERHEAT_SEN_ENABLED) {
        overheatSen.config(OVERHEAT_SEN_R, OVERHEAT_SEN_B, OVERHEAT_SEN_t, OVERHEAT_SEN_Rt);
        overheatSen.setPin(P_OVERHEAT_SEN, ADC_RESOLUTION);
    }

    gio::mode(P_HEATER, OUTPUT);

    loadSettings();
    initTimers();
}

void loop() {
    static auto currentTemp = 0.;
    static bool enabled = false;

    roomTempQueryTimer.tick();
    saveSettingsTimer.tick();

    if (roomTempQueryTimer.ready()) {
        currentTemp = tempSen.readTemperature();
    }

    if (saveSettingsTimer.ready()) {
        saveSettings();
    }

    if constexpr (OVERHEAT_SEN_ENABLED) {
        if (overheatQueryTimer.ready()) {
            overheating = overheatSen.getTempAverage() >= OVERHEAT_THRESHOLD;
        }
    }

    disp.update();
    disp.brightness(state.displayBrightness);

    enc.tick();

    processEncoderTurn();

    if (enc.click()) {
        state.nextScreen();
        saveSettingsTimer.start();
    }

    renderCurrentScreen(currentTemp);

    if (currentTemp >= (state.hysterDia + state.setTemp)) {
        enabled = false;
    } else if (currentTemp <= (state.setTemp - state.hysterDia)) {
        enabled = true;
    }

    gio::write(P_HEATER, (enabled && !overheating) ? 1 : 0);
}

#pragma clang diagnostic pop
