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

#include "macro.h"
#include "utils/SegBuffer.h"

namespace detail {
    FORCEINLINE constexpr auto countDigits(uint16_t value) {
        if (value < 10) return 1;
        return countDigits(value / 10) + 1;
    }

    FORCEINLINE constexpr auto countDigits(uint8_t value) {
        return countDigits(static_cast<uint16_t>(value));
    }

    FORCEINLINE constexpr auto countDigits(int16_t value) {
        return countDigits(static_cast<uint16_t>(abs(value))) + (value < 0 ? 1 : 0);
    }

    FORCEINLINE constexpr auto countDigits(double value) {
        return countDigits(static_cast<int16_t>(floor(value)));
    }
}

template<typename T>
FORCEINLINE void displayValue(SegBuffer &disp, char marker, T value) {
    disp.setCursor(0);

    if (detail::countDigits(value) < disp.getSize()) {
        disp.print(marker);
    }

    disp.printRight(true);
    disp.print(value);
    disp.printRight(false);
}

template<>
FORCEINLINE_NOSTATIC void displayValue(SegBuffer &disp, char marker, double value) {
    disp.setCursor(0);

    auto digits = detail::countDigits(value);

    if (digits < disp.getSize()) {
        disp.print(marker);
    }

    if (digits >= disp.getSize() - 1) {
        disp.print(value, 0);
    } else {
        disp.print(value, disp.getSize() - digits);
    }
}
