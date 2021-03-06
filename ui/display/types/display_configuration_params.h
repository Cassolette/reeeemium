// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_DISPLAY_TYPES_DISPLAY_CONFIGURATION_PARAMS_H_
#define UI_DISPLAY_TYPES_DISPLAY_CONFIGURATION_PARAMS_H_

#include <stdint.h>

#include "base/optional.h"
#include "ui/display/types/display_mode.h"
#include "ui/display/types/display_types_export.h"
#include "ui/gfx/geometry/point.h"

namespace display {

struct DISPLAY_TYPES_EXPORT DisplayConfigurationParams {
  DisplayConfigurationParams();
  DisplayConfigurationParams(int64_t id,
                             gfx::Point origin,
                             const display::DisplayMode* pmode);
  ~DisplayConfigurationParams();

  int64_t id = 0;
  gfx::Point origin = gfx::Point();
  base::Optional<std::unique_ptr<display::DisplayMode>> mode = base::nullopt;
};

}  // namespace display

#endif  // UI_DISPLAY_TYPES_DISPLAY_CONFIGURATION_PARAMS_H_
