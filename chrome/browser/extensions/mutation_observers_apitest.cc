// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/extension_apitest.h"
#include "content/public/test/browser_test.h"

namespace extensions {

IN_PROC_BROWSER_TEST_F(ExtensionApiTest, MutationObservers) {
  ASSERT_TRUE(RunExtensionTest("mutation_observers")) << message_;
}

}  // namespace extensions
