// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NEARBY_SHARING_SHARE_TARGET_DISCOVERED_CALLBACK_H_
#define CHROME_BROWSER_NEARBY_SHARING_SHARE_TARGET_DISCOVERED_CALLBACK_H_

#include "base/observer_list_types.h"
#include "chrome/browser/nearby_sharing/share_target.h"

// Reports newly discovered devices.
class ShareTargetDiscoveredCallback : public base::CheckedObserver {
 public:
  virtual void OnShareTargetDiscovered(ShareTarget shareTarget) = 0;

  virtual void OnShareTargetLost(ShareTarget shareTarget) = 0;
};

#endif  // CHROME_BROWSER_NEARBY_SHARING_SHARE_TARGET_DISCOVERED_CALLBACK_H_
