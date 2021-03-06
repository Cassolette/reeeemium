// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/enterprise/browser_management/browser_management_status_provider.h"

#include "build/build_config.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/policy/browser_dm_token_storage.h"
#include "chrome/browser/policy/chrome_browser_policy_connector.h"
#include "chrome/browser/policy/profile_policy_connector.h"
#include "chrome/browser/profiles/profile.h"

#if defined(OS_WIN)
#include "components/policy/core/common/management/platform_management_status_provider_win.h"
#endif

BrowserCloudManagementStatusProvider::BrowserCloudManagementStatusProvider() =
    default;

BrowserCloudManagementStatusProvider::~BrowserCloudManagementStatusProvider() =
    default;

bool BrowserCloudManagementStatusProvider::IsManaged() {
#if !defined(OS_ANDROID)
  return policy::BrowserDMTokenStorage::Get()->RetrieveDMToken().is_valid();
#else
  return false;
#endif
}

EnterpriseManagementAuthority
BrowserCloudManagementStatusProvider::GetAuthority() {
  return EnterpriseManagementAuthority::CLOUD_DOMAIN;
}

LocalBrowserManagementStatusProvider::LocalBrowserManagementStatusProvider() =
    default;

LocalBrowserManagementStatusProvider::~LocalBrowserManagementStatusProvider() =
    default;

bool LocalBrowserManagementStatusProvider::IsManaged() {
  return g_browser_process->browser_policy_connector()
      ->HasMachineLevelPolicies();
}

EnterpriseManagementAuthority
LocalBrowserManagementStatusProvider::GetAuthority() {
#if defined(OS_WIN)
  if (policy::DomainEnrollmentStatusProvider::IsEnrolledToDomain())
    return EnterpriseManagementAuthority::DOMAIN_LOCAL;
#endif
  return EnterpriseManagementAuthority::COMPUTER_LOCAL;
}

ProfileCloudManagementStatusProvider::ProfileCloudManagementStatusProvider(
    Profile* profile)
    : profile_(profile) {}

ProfileCloudManagementStatusProvider::~ProfileCloudManagementStatusProvider() =
    default;

bool ProfileCloudManagementStatusProvider::IsManaged() {
  return profile_->GetProfilePolicyConnector()->IsManaged();
}

EnterpriseManagementAuthority
ProfileCloudManagementStatusProvider::GetAuthority() {
  return EnterpriseManagementAuthority::CLOUD;
}
