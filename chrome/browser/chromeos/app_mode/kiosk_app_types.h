// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_APP_MODE_KIOSK_APP_TYPES_H_
#define CHROME_BROWSER_CHROMEOS_APP_MODE_KIOSK_APP_TYPES_H_

#include <string>

#include "base/optional.h"
#include "components/account_id/account_id.h"

namespace chromeos {

// Type of different kiosk apps to be launched.
enum class KioskAppType { ARC_APP, CHROME_APP, WEB_APP };

// Universal identifier for all kiosk apps.
class KioskAppId {
 public:
  KioskAppType type;
  base::Optional<std::string> app_id;
  base::Optional<AccountId> account_id;

  KioskAppId();
  ~KioskAppId();
  KioskAppId(const KioskAppId&);

  static KioskAppId ForChromeApp(const std::string& app_id);
  static KioskAppId ForWebApp(const AccountId& account_id);
  static KioskAppId ForArcApp(const AccountId& account_id);

 private:
  KioskAppId(KioskAppType type, const std::string& app_id);
  KioskAppId(KioskAppType type, const AccountId& account_id);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_APP_MODE_KIOSK_APP_TYPES_H_
