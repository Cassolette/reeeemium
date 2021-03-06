// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_COMPONENTS_MEDIA_APP_UI_MEDIA_APP_GUEST_UI_H_
#define CHROMEOS_COMPONENTS_MEDIA_APP_UI_MEDIA_APP_GUEST_UI_H_

namespace content {
class WebUIDataSource;
}

class MediaAppUIDelegate;

namespace chromeos {

// The data source for chrome-untrusted://media-app.
content::WebUIDataSource* CreateMediaAppUntrustedDataSource(
    MediaAppUIDelegate* delegate);

}  // namespace chromeos

#endif  // CHROMEOS_COMPONENTS_MEDIA_APP_UI_MEDIA_APP_GUEST_UI_H_
