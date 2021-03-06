// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SERVICES_APP_SERVICE_PUBLIC_CPP_APP_REGISTRY_CACHE_WRAPPER_H_
#define COMPONENTS_SERVICES_APP_SERVICE_PUBLIC_CPP_APP_REGISTRY_CACHE_WRAPPER_H_

#include <map>

class AccountId;

namespace apps {

class AppRegistryCache;

// Wraps AppRegistryCache to get all AppRegistryCaches independently. Provides
// the method to get the AppRegistryCache per |account_id|.
class AppRegistryCacheWrapper {
 public:
  // Returns the global AppRegistryCacheWrapper object.
  static AppRegistryCacheWrapper& Get();

  AppRegistryCacheWrapper();
  ~AppRegistryCacheWrapper();

  AppRegistryCacheWrapper(const AppRegistryCacheWrapper&) = delete;
  AppRegistryCacheWrapper& operator=(const AppRegistryCacheWrapper&) = delete;

  // Returns AppRegistryCache for the given |account_id|, or return null if
  // AppRegistryCache doesn't exist.
  AppRegistryCache* GetAppRegistryCache(const AccountId& account_id);

  // Adds the AppRegistryCache for the given |account_id|.
  void AddAppRegistryCache(const AccountId& account_id,
                           AppRegistryCache* cache);

  // Removes the |cache| in |app_registry_caches_|.
  void RemoveAppRegistryCache(AppRegistryCache* cache);

 private:
  std::map<AccountId, AppRegistryCache*> app_registry_caches_;
};

}  // namespace apps

#endif  // COMPONENTS_SERVICES_APP_SERVICE_PUBLIC_CPP_APP_REGISTRY_CACHE_WRAPPER_H_
