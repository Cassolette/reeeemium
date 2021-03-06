// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/drive/drive_integration_service.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/test/bind_test_util.h"
#include "base/threading/thread_restrictions.h"
#include "chrome/browser/chromeos/drive/drivefs_test_support.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chromeos/constants/chromeos_switches.h"
#include "components/drive/drive_pref_names.h"
#include "components/prefs/pref_service.h"
#include "content/public/test/browser_test.h"

namespace drive {

class DriveIntegrationServiceBrowserTest : public InProcessBrowserTest {
  bool SetUpUserDataDirectory() override {
    return drive::SetUpUserDataDirectoryForDriveFsTest();
  }

  void SetUpInProcessBrowserTestFixture() override {
    create_drive_integration_service_ = base::Bind(
        &DriveIntegrationServiceBrowserTest::CreateDriveIntegrationService,
        base::Unretained(this));
    service_factory_for_test_ = std::make_unique<
        drive::DriveIntegrationServiceFactory::ScopedFactoryForTest>(
        &create_drive_integration_service_);
  }

 private:
  drive::DriveIntegrationService* CreateDriveIntegrationService(
      Profile* profile) {
    base::ScopedAllowBlockingForTesting allow_blocking;
    base::FilePath mount_path = profile->GetPath().Append("drivefs");
    fake_drivefs_helpers_[profile] =
        std::make_unique<drive::FakeDriveFsHelper>(profile, mount_path);
    auto* integration_service = new drive::DriveIntegrationService(
        profile, std::string(), mount_path,
        fake_drivefs_helpers_[profile]->CreateFakeDriveFsListenerFactory());
    return integration_service;
  }

  drive::DriveIntegrationServiceFactory::FactoryCallback
      create_drive_integration_service_;
  std::unique_ptr<drive::DriveIntegrationServiceFactory::ScopedFactoryForTest>
      service_factory_for_test_;
  std::map<Profile*, std::unique_ptr<drive::FakeDriveFsHelper>>
      fake_drivefs_helpers_;
};

// Verify DriveIntegrationService is created during login.
IN_PROC_BROWSER_TEST_F(DriveIntegrationServiceBrowserTest,
                       CreatedDuringLogin) {
  EXPECT_TRUE(DriveIntegrationServiceFactory::FindForProfile(
      browser()->profile()));
}

IN_PROC_BROWSER_TEST_F(DriveIntegrationServiceBrowserTest,
                       ClearCacheAndRemountFileSystem) {
  base::ScopedAllowBlockingForTesting allow_blocking;

  auto* drive_service =
      DriveIntegrationServiceFactory::FindForProfile(browser()->profile());
  base::FilePath cache_path = drive_service->GetDriveFsHost()->GetDataPath();
  base::FilePath log_folder_path = drive_service->GetDriveFsLogPath().DirName();
  ASSERT_TRUE(base::CreateDirectory(cache_path));
  ASSERT_TRUE(base::CreateDirectory(log_folder_path));

  base::FilePath cache_file;
  base::FilePath log_file;
  ASSERT_TRUE(base::CreateTemporaryFileInDir(cache_path, &cache_file));
  ASSERT_TRUE(base::CreateTemporaryFileInDir(log_folder_path, &log_file));

  base::RunLoop run_loop;
  auto quit_closure = run_loop.QuitClosure();
  drive_service->ClearCacheAndRemountFileSystem(
      base::BindLambdaForTesting([=](bool success) {
        EXPECT_TRUE(success);
        EXPECT_FALSE(base::PathExists(cache_file));
        EXPECT_TRUE(base::PathExists(log_file));
        quit_closure.Run();
      }));

  run_loop.Run();
}

IN_PROC_BROWSER_TEST_F(DriveIntegrationServiceBrowserTest,
                       DisableDrivePolicyTest) {
  // First make sure the pref is set to its default value which should permit
  // drive.
  browser()->profile()->GetPrefs()->SetBoolean(prefs::kDisableDrive, false);

  drive::DriveIntegrationService* integration_service =
      drive::DriveIntegrationServiceFactory::FindForProfile(
          browser()->profile());

  EXPECT_TRUE(integration_service);
  EXPECT_TRUE(integration_service->is_enabled());

  // ...next try to disable drive.
  browser()->profile()->GetPrefs()->SetBoolean(prefs::kDisableDrive, true);

  EXPECT_EQ(integration_service,
            drive::DriveIntegrationServiceFactory::FindForProfile(
                browser()->profile()));
  EXPECT_FALSE(integration_service->is_enabled());
}

class DriveIntegrationServiceWithGaiaDisabledBrowserTest
    : public DriveIntegrationServiceBrowserTest {
  void SetUpCommandLine(base::CommandLine* command_line) override {
    command_line->AppendSwitch(chromeos::switches::kDisableGaiaServices);
  }
};

IN_PROC_BROWSER_TEST_F(DriveIntegrationServiceWithGaiaDisabledBrowserTest,
                       DriveDisabled) {
  // First make sure the pref is set to its default value which would normally
  // permit drive.
  browser()->profile()->GetPrefs()->SetBoolean(prefs::kDisableDrive, false);

  drive::DriveIntegrationService* integration_service =
      drive::DriveIntegrationServiceFactory::FindForProfile(
          browser()->profile());

  ASSERT_TRUE(integration_service);
  EXPECT_FALSE(integration_service->is_enabled());
}

IN_PROC_BROWSER_TEST_F(DriveIntegrationServiceBrowserTest, GetMetadata) {
  base::ScopedAllowBlockingForTesting allow_blocking;
  auto* drive_service =
      DriveIntegrationServiceFactory::FindForProfile(browser()->profile());

  base::FilePath mount_path = drive_service->GetMountPointPath();
  base::FilePath file_path;
  base::CreateTemporaryFileInDir(mount_path, &file_path);

  {
    base::RunLoop run_loop;
    auto quit_closure = run_loop.QuitClosure();
    drive_service->GetMetadata(
        base::FilePath("/foo/bar"),
        base::BindLambdaForTesting(
            [=](FileError error, drivefs::mojom::FileMetadataPtr metadata_ptr) {
              EXPECT_EQ(FILE_ERROR_NOT_FOUND, error);
              quit_closure.Run();
            }));
    run_loop.Run();
  }

  {
    base::RunLoop run_loop;
    auto quit_closure = run_loop.QuitClosure();
    drive_service->GetMetadata(
        file_path,
        base::BindLambdaForTesting(
            [=](FileError error, drivefs::mojom::FileMetadataPtr metadata_ptr) {
              EXPECT_EQ(FILE_ERROR_OK, error);
              quit_closure.Run();
            }));
    run_loop.Run();
  }
}
}  // namespace drive
