// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_NTP_INCOGNITO_VIEW_CONTROLLER_H_
#define IOS_CHROME_BROWSER_UI_NTP_INCOGNITO_VIEW_CONTROLLER_H_

#import <UIKit/UIKit.h>

#import "ios/chrome/browser/ui/settings/privacy/cookies_consumer.h"

@protocol NewTabPageControllerDelegate;
@protocol PrivacyCookiesCommands;
class UrlLoadingBrowserAgent;

@interface IncognitoViewController : UIViewController <PrivacyCookiesConsumer>

// Init with the given loader object. |loader| may be nil, but isn't
// retained so it must outlive this controller.
- (instancetype)initWithUrlLoader:(UrlLoadingBrowserAgent*)URLLoader;

// Handler used to update Cookies settings.
@property(nonatomic, weak) id<PrivacyCookiesCommands> handler;

@end

#endif  // IOS_CHROME_BROWSER_UI_NTP_INCOGNITO_VIEW_CONTROLLER_H_
