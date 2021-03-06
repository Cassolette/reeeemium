// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBLAYER_BROWSER_TEST_TEST_INFOBAR_H_
#define WEBLAYER_BROWSER_TEST_TEST_INFOBAR_H_

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"
#include "components/infobars/core/infobar_delegate.h"
#include "content/public/browser/web_contents.h"
#include "weblayer/browser/infobar_android.h"

namespace weblayer {

class TestInfoBarDelegate;

// A test infobar.
class TestInfoBar : public InfoBarAndroid {
 public:
  explicit TestInfoBar(std::unique_ptr<TestInfoBarDelegate> delegate);
  ~TestInfoBar() override;

  TestInfoBar(const TestInfoBar&) = delete;
  TestInfoBar& operator=(const TestInfoBar&) = delete;

  static void Show(content::WebContents* web_contents);

 protected:
  infobars::InfoBarDelegate* GetDelegate();

  // InfoBarAndroid overrides.
  void ProcessButton(int action) override;
  base::android::ScopedJavaLocalRef<jobject> CreateRenderInfoBar(
      JNIEnv* env) override;

 private:
};

}  // namespace weblayer

#endif  // WEBLAYER_BROWSER_TEST_TEST_INFOBAR_H_
