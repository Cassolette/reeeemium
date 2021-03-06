// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/idle/idle_query_x11.h"

#include "ui/gfx/x/connection.h"
#include "ui/gfx/x/screensaver.h"
#include "ui/gfx/x/x11.h"
#include "ui/gfx/x/x11_types.h"

namespace ui {

IdleQueryX11::IdleQueryX11() : connection_(x11::Connection::Get()) {
  // Let the server know the client version before making any requests.
  connection_->screensaver().QueryVersion(
      {x11::ScreenSaver::major_version, x11::ScreenSaver::minor_version});
}

IdleQueryX11::~IdleQueryX11() = default;

int IdleQueryX11::IdleTime() {
  if (auto reply = connection_->screensaver()
                       .QueryInfo({connection_->default_root()})
                       .Sync()) {
    return reply->ms_since_user_input / 1000;
  }
  return 0;
}

}  // namespace ui
