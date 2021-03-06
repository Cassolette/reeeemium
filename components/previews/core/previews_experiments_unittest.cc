// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/previews/core/previews_experiments.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/feature_list.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/field_trial_params.h"
#include "base/strings/string_util.h"
#include "base/test/scoped_feature_list.h"
#include "build/build_config.h"
#include "components/previews/core/previews_features.h"
#include "components/variations/variations_associated_data.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace previews {

namespace {

const char kClientSidePreviewsFieldTrial[] = "ClientSidePreviews";
const char kPreviewsFieldTrial[] = "Previews";
const char kEnabled[] = "Enabled";

// Verifies that the default params are correct, and that custom params can be
// set, for both the previews blocklist and offline previews.
TEST(PreviewsExperimentsTest, TestParamsForBlockListAndOffline) {
  // Verify that the default params are correct.
  EXPECT_EQ(4u, params::MaxStoredHistoryLengthForPerHostBlockList());
  EXPECT_EQ(10u, params::MaxStoredHistoryLengthForHostIndifferentBlockList());
  EXPECT_EQ(100u, params::MaxInMemoryHostsInBlockList());
  EXPECT_EQ(2, params::PerHostBlockListOptOutThreshold());
  EXPECT_EQ(6, params::HostIndifferentBlockListOptOutThreshold());
  EXPECT_EQ(base::TimeDelta::FromDays(30), params::PerHostBlockListDuration());
  EXPECT_EQ(base::TimeDelta::FromDays(30),
            params::HostIndifferentBlockListPerHostDuration());
  EXPECT_EQ(base::TimeDelta::FromSeconds(60 * 5),
            params::SingleOptOutDuration());
  EXPECT_EQ(base::TimeDelta::FromDays(7),
            params::OfflinePreviewFreshnessDuration());
  EXPECT_EQ(net::EFFECTIVE_CONNECTION_TYPE_2G,
            params::GetECTThresholdForPreview(PreviewsType::OFFLINE));
  EXPECT_EQ(net::EFFECTIVE_CONNECTION_TYPE_2G,
            params::GetECTThresholdForPreview(PreviewsType::NOSCRIPT));
  EXPECT_EQ(0, params::OfflinePreviewsVersion());

  // Set some custom params. Somewhat random yet valid values.
  // TODO(crbug.com/1092102) : Migrate blacklist names to blocklist.
  std::map<std::string, std::string> custom_params = {
      {"per_host_max_stored_history_length", "3"},
      {"host_indifferent_max_stored_history_length", "4"},
      {"max_hosts_in_blacklist", "13"},
      {"per_host_opt_out_threshold", "12"},
      {"host_indifferent_opt_out_threshold", "84"},
      {"per_host_black_list_duration_in_days", "99"},
      {"host_indifferent_black_list_duration_in_days", "64"},
      {"single_opt_out_duration_in_seconds", "28"},
      {"offline_preview_freshness_duration_in_days", "12"},
      {"max_allowed_effective_connection_type", "4G"},
      {"version", "10"},
  };
  EXPECT_TRUE(base::AssociateFieldTrialParams(kClientSidePreviewsFieldTrial,
                                              kEnabled, custom_params));
  EXPECT_TRUE(base::FieldTrialList::CreateFieldTrial(
      kClientSidePreviewsFieldTrial, kEnabled));

  EXPECT_EQ(3u, params::MaxStoredHistoryLengthForPerHostBlockList());
  EXPECT_EQ(4u, params::MaxStoredHistoryLengthForHostIndifferentBlockList());
  EXPECT_EQ(13u, params::MaxInMemoryHostsInBlockList());
  EXPECT_EQ(12, params::PerHostBlockListOptOutThreshold());
  EXPECT_EQ(84, params::HostIndifferentBlockListOptOutThreshold());
  EXPECT_EQ(base::TimeDelta::FromDays(99), params::PerHostBlockListDuration());
  EXPECT_EQ(base::TimeDelta::FromDays(64),
            params::HostIndifferentBlockListPerHostDuration());
  EXPECT_EQ(base::TimeDelta::FromSeconds(28), params::SingleOptOutDuration());
  EXPECT_EQ(base::TimeDelta::FromDays(12),
            params::OfflinePreviewFreshnessDuration());
  EXPECT_EQ(net::EFFECTIVE_CONNECTION_TYPE_2G,
            params::GetECTThresholdForPreview(PreviewsType::OFFLINE));
  EXPECT_EQ(net::EFFECTIVE_CONNECTION_TYPE_2G,
            params::GetECTThresholdForPreview(PreviewsType::NOSCRIPT));
  EXPECT_EQ(10, params::OfflinePreviewsVersion());

  variations::testing::ClearAllVariationParams();
}

TEST(PreviewsExperimentsTest, TestDefaultShouldExcludeMediaSuffix) {
  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitAndEnableFeature(features::kExcludedMediaSuffixes);

  EXPECT_FALSE(
      params::ShouldExcludeMediaSuffix(GURL("http://chromium.org/path/")));

  std::vector<std::string> default_suffixes = {
      ".apk", ".avi",  ".gif", ".gifv", ".jpeg", ".jpg", ".mp3",
      ".mp4", ".mpeg", ".pdf", ".png",  ".webm", ".webp"};
  for (const std::string& suffix : default_suffixes) {
    GURL url("http://chromium.org/path/" + suffix);
    EXPECT_TRUE(params::ShouldExcludeMediaSuffix(url));
  }
}

TEST(PreviewsExperimentsTest, TestShouldExcludeMediaSuffix) {
  struct TestCase {
    std::string msg;
    bool enable_feature;
    std::string varaiation_value;
    std::vector<std::string> urls;
    bool want_return;
  };
  const TestCase kTestCases[]{
      {
          .msg = "Feature disabled, should always return false",
          .enable_feature = false,
          .varaiation_value = "",
          .urls = {"http://chromium.org/video.mp4"},
          .want_return = false,
      },
      {
          .msg = "Default values are overridden by variations",
          .enable_feature = true,
          .varaiation_value = ".html",
          .urls = {"http://chromium.org/video.mp4",
                   "http://chromium.org/image.png",
                   "http://chromium.org/image.jpg",
                   "http://chromium.org/audio.mp3"},
          .want_return = false,
      },
      {
          .msg = "Variation value whitespace should be trimmed",
          .enable_feature = true,
          .varaiation_value = " .mp4 , \t .png\n",
          .urls = {"http://chromium.org/video.mp4",
                   "http://chromium.org/image.png"},
          .want_return = true,
      },
      {
          .msg = "Variation value empty values should be excluded",
          .enable_feature = true,
          .varaiation_value = ".mp4,,.png,",
          .urls = {"http://chromium.org/video.mp4",
                   "http://chromium.org/image.png"},
          .want_return = true,
      },
      {
          .msg = "URLs should be compared case insensitive",
          .enable_feature = true,
          .varaiation_value = ".MP4,.png,",
          .urls = {"http://chromium.org/video.mP4",
                   "http://chromium.org/image.PNG"},
          .want_return = true,
      },
      {
          .msg = "Query params and fragments don't matter",
          .enable_feature = true,
          .varaiation_value = ".mp4,.png,",
          .urls = {"http://chromium.org/video.mp4?hello=world",
                   "http://chromium.org/image.png#test"},
          .want_return = true,
      },
      {
          .msg = "Query params and fragments shouldn't be considered",
          .enable_feature = true,
          .varaiation_value = ".mp4,.png,",
          .urls = {"http://chromium.org/?video=video.mp4",
                   "http://chromium.org/#image.png"},
          .want_return = false,
      },
  };
  for (const TestCase& test_case : kTestCases) {
    SCOPED_TRACE(test_case.msg);

    base::test::ScopedFeatureList scoped_feature_list;
    if (test_case.enable_feature) {
      scoped_feature_list.InitAndEnableFeatureWithParameters(
          features::kExcludedMediaSuffixes,
          {{"excluded_path_suffixes", test_case.varaiation_value}});
    } else {
      scoped_feature_list.InitAndDisableFeature(
          features::kExcludedMediaSuffixes);
    }

    for (const std::string& url : test_case.urls) {
      EXPECT_EQ(test_case.want_return,
                params::ShouldExcludeMediaSuffix(GURL(url)));
    }
  }
}

TEST(PreviewsExperimentsTest, TestPreviewsEligibleForUserConsistentStudy) {
  EXPECT_TRUE(base::FieldTrialList::CreateFieldTrial("CoinFlipPreviews",
                                                     "NonCoinFlipGroup"));

  // Base features enabled + EligibleForUserConsistentStudy +
  // UserConsistent features disabled.
  {
    base::test::ScopedFeatureList scoped_feature_list;
    scoped_feature_list.InitWithFeatures(
        {features::kDeferAllScriptPreviews, features::kResourceLoadingHints,
         features::kNoScriptPreviews,
         features::kEligibleForUserConsistentStudy},
        {features::kDeferAllScriptPreviewsUserConsistentStudy,
         features::kResourceLoadingHintsUserConsistentStudy,
         features::kNoScriptPreviewsUserConsistentStudy});

    EXPECT_TRUE(params::IsDeferAllScriptPreviewsEnabled());
    EXPECT_TRUE(params::IsResourceLoadingHintsEnabled());
    EXPECT_TRUE(params::IsNoScriptPreviewsEnabled());
  }

  // Base features disabled + EligibleForUserConsistentStudy +
  // UserConsistent features enabled but do not specify enabled parameter.
  {
    base::test::ScopedFeatureList scoped_feature_list;
    scoped_feature_list.InitWithFeatures(
        {features::kEligibleForUserConsistentStudy,
         features::kDeferAllScriptPreviewsUserConsistentStudy,
         features::kResourceLoadingHintsUserConsistentStudy,
         features::kNoScriptPreviewsUserConsistentStudy},
        {features::kDeferAllScriptPreviews, features::kResourceLoadingHints,
         features::kNoScriptPreviews});

    EXPECT_FALSE(params::IsDeferAllScriptPreviewsEnabled());
    EXPECT_FALSE(params::IsResourceLoadingHintsEnabled());
    EXPECT_FALSE(params::IsNoScriptPreviewsEnabled());
  }

  // Base features disabled + EligibleForUserConsistentStudy +
  // UserConsistent features enabled with enabled parameter true.
  {
    base::test::ScopedFeatureList scoped_feature_list;
    scoped_feature_list.InitWithFeaturesAndParameters(
        {{features::kEligibleForUserConsistentStudy, {}},
         {features::kDeferAllScriptPreviewsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "true"}, {"version", "333"}}},
         {features::kResourceLoadingHintsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "true"}, {"version", "555"}}},
         {features::kNoScriptPreviewsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "true"}, {"version", "777"}}}},
        {features::kDeferAllScriptPreviews, features::kResourceLoadingHints,
         features::kNoScriptPreviews});

    EXPECT_TRUE(params::IsDeferAllScriptPreviewsEnabled());
    EXPECT_TRUE(params::IsResourceLoadingHintsEnabled());
    EXPECT_TRUE(params::IsNoScriptPreviewsEnabled());

    // Verify the UserConsistent feature version params.
    EXPECT_EQ(333, params::DeferAllScriptPreviewsVersion());
    EXPECT_EQ(555, params::ResourceLoadingHintsVersion());
    EXPECT_EQ(777, params::NoScriptPreviewsVersion());
  }

  // Base features disabled + EligibleForUserConsistentStudy +
  // UserConsistent features enabled with enabled parameter false.
  {
    base::test::ScopedFeatureList scoped_feature_list;
    scoped_feature_list.InitWithFeaturesAndParameters(
        {{features::kEligibleForUserConsistentStudy, {}},
         {features::kDeferAllScriptPreviewsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "false"}, {"version", "333"}}},
         {features::kResourceLoadingHintsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "false"}, {"version", "555"}}},
         {features::kNoScriptPreviewsUserConsistentStudy,
          {{"user_consistent_preview_enabled", "false"}, {"version", "777"}}}},
        {features::kDeferAllScriptPreviews, features::kResourceLoadingHints,
         features::kNoScriptPreviews});

    EXPECT_FALSE(params::IsDeferAllScriptPreviewsEnabled());
    EXPECT_FALSE(params::IsResourceLoadingHintsEnabled());
    EXPECT_FALSE(params::IsNoScriptPreviewsEnabled());
  }
}

TEST(PreviewsExperimentsTest, TestDeferAllScriptPreviewsCoinFlipExperiment) {
  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitWithFeaturesAndParameters(
      {{features::kDeferAllScriptPreviewsUserConsistentStudy,
        {{"user_consistent_preview_enabled", "true"}, {"version", "333"}}},
       {features::kDeferAllScriptPreviews, {{"version", "444"}}}},
      {features::kEligibleForUserConsistentStudy});

  EXPECT_TRUE(base::FieldTrialList::CreateFieldTrial(
      kPreviewsFieldTrial, "DeferAllScriptPreviewsCoinFlipExperimentGroup"));

  EXPECT_TRUE(params::IsDeferAllScriptPreviewsEnabled());
  EXPECT_EQ(444, params::DeferAllScriptPreviewsVersion());
}

TEST(PreviewsExperimentsTest, TestOverrideShouldShowPreviewCheck) {
  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitWithFeaturesAndParameters(
      {{features::kPreviews, {{"override_should_show_preview_check", "true"}}}},
      {});

  EXPECT_TRUE(params::OverrideShouldShowPreviewCheck());
}

}  // namespace

}  // namespace previews
