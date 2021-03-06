// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/saml/saml_profile_prefs.h"

#include "chrome/common/pref_names.h"
#include "chromeos/login/auth/saml_password_attributes.h"
#include "components/pref_registry/pref_registry_syncable.h"

namespace {

const int kDefaultSAMLOfflineSigninTimeLimit = 14 * 24 * 60 * 60;  // 14 days.

// In-session password-change feature (includes password expiry notifications).
const bool kDefaultSamlInSessionPasswordChangeEnabled = false;
const int kDefaultSamlPasswordExpirationAdvanceWarningDays = 14;

// Online reauthentication on the lock screen.
const bool kDefaultSamlLockScreenReauthenticationEnabled = false;

}  // namespace

namespace chromeos {

void RegisterSamlProfilePrefs(user_prefs::PrefRegistrySyncable* registry) {
  registry->RegisterIntegerPref(prefs::kSAMLOfflineSigninTimeLimit,
                                kDefaultSAMLOfflineSigninTimeLimit);
  registry->RegisterTimePref(prefs::kSAMLLastGAIASignInTime, base::Time());

  registry->RegisterBooleanPref(prefs::kSamlInSessionPasswordChangeEnabled,
                                kDefaultSamlInSessionPasswordChangeEnabled);
  registry->RegisterIntegerPref(
      prefs::kSamlPasswordExpirationAdvanceWarningDays,
      kDefaultSamlPasswordExpirationAdvanceWarningDays);

  registry->RegisterBooleanPref(prefs::kSamlLockScreenReauthenticationEnabled,
                                kDefaultSamlLockScreenReauthenticationEnabled);

  SamlPasswordAttributes::RegisterProfilePrefs(registry);
}

}  // namespace chromeos
