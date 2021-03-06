// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBLAYER_PUBLIC_PROFILE_H_
#define WEBLAYER_PUBLIC_PROFILE_H_

#include <memory>
#include <string>

#include "base/callback_forward.h"
#include "base/containers/flat_set.h"
#include "base/time/time.h"

namespace base {
class FilePath;
}

namespace weblayer {
class CookieManager;
class DownloadDelegate;

// GENERATED_JAVA_ENUM_PACKAGE: org.chromium.weblayer_private
// GENERATED_JAVA_CLASS_NAME_OVERRIDE: ImplBrowsingDataType
enum class BrowsingDataType {
  COOKIES_AND_SITE_DATA = 0,
  CACHE = 1,
};

// Used for setting/getting profile related settings.
enum class SettingType {
  BASIC_SAFE_BROWSING_ENABLED = 0,
  UKM_ENABLED = 1,
  EXTENDED_REPORTING_SAFE_BROWSING_ENABLED = 2,
  REAL_TIME_SAFE_BROWSING_ENABLED = 3,
};

class Profile {
 public:
  // Pass an empty |name| for an in-memory profile.
  // Otherwise, |name| should contain only alphanumeric characters and
  // underscore.
  static std::unique_ptr<Profile> Create(const std::string& name);

  // Delete all profile's data from disk. If there are any existing usage
  // of this profile, return |profile| immediately and |done_callback| will not
  // be called. Otherwise return nullptr and |done_callback| is called when
  // deletion is complete.
  static std::unique_ptr<Profile> DestroyAndDeleteDataFromDisk(
      std::unique_ptr<Profile> profile,
      base::OnceClosure done_callback);

  virtual ~Profile() {}

  virtual void ClearBrowsingData(
      const std::vector<BrowsingDataType>& data_types,
      base::Time from_time,
      base::Time to_time,
      base::OnceClosure callback) = 0;

  // Allows embedders to override the default download directory, which is the
  // system download directory on Android and on other platforms it's in the
  // home directory.
  virtual void SetDownloadDirectory(const base::FilePath& directory) = 0;

  // Sets the DownloadDelegate. If none is set, downloads will be dropped.
  virtual void SetDownloadDelegate(DownloadDelegate* delegate) = 0;

  // Gets the cookie manager for this profile.
  virtual CookieManager* GetCookieManager() = 0;

  // Asynchronously fetches the set of known Browser persistence-ids. See
  // Browser::PersistenceInfo for more details on persistence-ids.
  virtual void GetBrowserPersistenceIds(
      base::OnceCallback<void(base::flat_set<std::string>)> callback) = 0;

  // Asynchronously removes the storage associated with the set of
  // Browser persistence-ids. This ignores ids actively in use. |done_callback|
  // is run with the result of the operation (on the main thread). A value of
  // true means all files were removed. A value of false indicates at least one
  // of the files could not be removed.
  virtual void RemoveBrowserPersistenceStorage(
      base::OnceCallback<void(bool)> done_callback,
      base::flat_set<std::string> ids) = 0;

  // Set the boolean value of the given setting type.
  virtual void SetBooleanSetting(SettingType type, bool value) = 0;

  // Get the boolean value of the given setting type.
  virtual bool GetBooleanSetting(SettingType type) = 0;

  // For cross-origin navigations, the implementation may leverage a separate OS
  // process for stronger isolation. If an embedder knows that a cross-origin
  // navigation is likely starting soon, they can call this method as a hint to
  // the implementation to start a fresh OS process. A subsequent navigation may
  // use this preinitialized process, improving performance. It is safe to call
  // this multiple times or when it is not certain that the spare renderer will
  // be used, although calling this too eagerly may reduce performance as
  // unnecessary processes are created.
  virtual void PrepareForPossibleCrossOriginNavigation() = 0;
};

}  // namespace weblayer

#endif  // WEBLAYER_PUBLIC_PROFILE_H_
