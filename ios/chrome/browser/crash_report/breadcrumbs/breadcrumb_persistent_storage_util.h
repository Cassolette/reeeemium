// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_CRASH_REPORT_BREADCRUMBS_BREADCRUMB_PERSISTENT_STORAGE_UTIL_H_
#define IOS_CHROME_BROWSER_CRASH_REPORT_BREADCRUMBS_BREADCRUMB_PERSISTENT_STORAGE_UTIL_H_

#include "base/files/file_path.h"

namespace web {
class BrowserState;
}  // namespace web

namespace breadcrumb_persistent_storage_util {

// Returns the path to a file for storing breadcrumbs within |browser_state|'s
// storage directory.
base::FilePath GetBreadcrumbPersistentStorageFilePath(
    web::BrowserState* browser_state);

// Returns the path to a file for storing breadcrumbs within |browser_state|'s
// storage directory. This second file is used to write the new breadcrumbs to
// so that the primary breadcrumbs file at
// |GetBreadcrumbPersistentStorageFilePath()| is always in a state correctly
// describing the application. (If the contents of a single file was instead
// cleared and re-written, the most recent breadcrumbs would be missing if the
// application crashed during this timeframe which will happen often whenever
// old breadcrumbs are removed.)
base::FilePath GetBreadcrumbPersistentStorageTempFilePath(
    web::BrowserState* browser_state);

}  // namespace breadcrumb_persistent_storage_util

#endif  // IOS_CHROME_BROWSER_CRASH_REPORT_BREADCRUMBS_BREADCRUMB_PERSISTENT_STORAGE_UTIL_H_
