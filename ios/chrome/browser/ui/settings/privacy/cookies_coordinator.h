// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_SETTINGS_PRIVACY_COOKIES_COORDINATOR_H_
#define IOS_CHROME_BROWSER_UI_SETTINGS_PRIVACY_COOKIES_COORDINATOR_H_

#import <Foundation/Foundation.h>

#import "ios/chrome/browser/ui/coordinators/chrome_coordinator.h"

@class PrivacyCookiesCoordinator;

// Delegate that allows to dereference the PrivacyCookiesCoordinator.
@protocol PrivacyCookiesCoordinatorDelegate

@optional
// Called when the view controller is removed from navigation controller.
- (void)privacyCookiesCoordinatorViewControllerWasRemoved:
    (PrivacyCookiesCoordinator*)coordinator;

// Called when the view controller should be dismissed.
- (void)dismissPrivacyCookiesCoordinatorViewController:
    (PrivacyCookiesCoordinator*)coordinator;

@end
// The coordinator for the Cookies screen.
@interface PrivacyCookiesCoordinator : ChromeCoordinator

@property(nonatomic, weak) id<PrivacyCookiesCoordinatorDelegate> delegate;

- (instancetype)initWithBaseNavigationController:
                    (UINavigationController*)navigationController
                                         browser:(Browser*)browser;

@end

#endif  // IOS_CHROME_BROWSER_UI_SETTINGS_PRIVACY_COOKIES_COORDINATOR_H_
