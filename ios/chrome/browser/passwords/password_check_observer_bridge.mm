// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ios/chrome/browser/passwords/password_check_observer_bridge.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

PasswordCheckObserverBridge::PasswordCheckObserverBridge(
    id<PasswordCheckObserver> delegate,
    IOSChromePasswordCheckManager* manager)
    : delegate_(delegate) {
  DCHECK(manager);
  password_check_manager_observer_.Add(manager);
}

PasswordCheckObserverBridge::~PasswordCheckObserverBridge() = default;

void PasswordCheckObserverBridge::PasswordCheckStatusChanged(
    PasswordCheckState status) {
  [delegate_ passwordCheckStateDidChange:status];
}

void PasswordCheckObserverBridge::CompromisedCredentialsChanged(
    password_manager::CompromisedCredentialsManager::CredentialsView
        credentials) {
  [delegate_ compromisedCredentialsDidChange:credentials];
}
