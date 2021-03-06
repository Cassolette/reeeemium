// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/overlays/infobar_modal/translate/translate_infobar_modal_overlay_mediator.h"

#include "base/strings/sys_string_conversions.h"
#include "base/test/scoped_feature_list.h"
#include "components/infobars/core/infobar_feature.h"
#include "ios/chrome/browser/infobars/infobar_ios.h"
#import "ios/chrome/browser/overlays/public/infobar_modal/infobar_modal_overlay_responses.h"
#import "ios/chrome/browser/overlays/public/infobar_modal/translate_infobar_modal_overlay_request_config.h"
#import "ios/chrome/browser/overlays/public/infobar_modal/translate_infobar_modal_overlay_responses.h"
#include "ios/chrome/browser/overlays/public/overlay_callback_manager.h"
#include "ios/chrome/browser/overlays/public/overlay_request.h"
#include "ios/chrome/browser/overlays/public/overlay_response.h"
#include "ios/chrome/browser/overlays/test/fake_overlay_request_callback_installer.h"
#import "ios/chrome/browser/translate/fake_translate_infobar_delegate.h"
#import "ios/chrome/browser/ui/infobars/coordinators/infobar_translate_modal_consumer.h"
#import "ios/chrome/browser/ui/infobars/infobar_feature.h"
#import "ios/chrome/browser/ui/infobars/modals/test/fake_infobar_translate_modal_consumer.h"
#import "ios/chrome/browser/ui/infobars/test/fake_infobar_ui_delegate.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest_mac.h"
#include "testing/platform_test.h"
#import "third_party/ocmock/OCMock/OCMock.h"
#import "third_party/ocmock/gtest_support.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

using translate_infobar_overlays::TranslateModalRequestConfig;
using translate_infobar_modal_responses::RevertTranslation;
using translate_infobar_modal_responses::ToggleAlwaysTranslate;
using translate_infobar_modal_responses::ToggleNeverTranslateSourceLanguage;
using translate_infobar_modal_responses::ToggleBlacklistSite;
using translate_infobar_modal_responses::UpdateLanguageInfo;
using translate_infobar_modal_responses::UpdateLanguageInfo;

// Test fixture for TranslateInfobarModalOverlayMediator.
class TranslateInfobarModalOverlayMediatorTest : public PlatformTest {
 public:
  TranslateInfobarModalOverlayMediatorTest()
      : infobar_(
            [[FakeInfobarUIDelegate alloc] init],
            delegate_factory_.CreateFakeTranslateInfoBarDelegate("fr", "en")),
        callback_installer_(
            &callback_receiver_,
            {InfobarModalMainActionResponse::ResponseSupport(),
             RevertTranslation::ResponseSupport(),
             ToggleNeverTranslateSourceLanguage::ResponseSupport(),
             ToggleBlacklistSite::ResponseSupport(),
             ToggleAlwaysTranslate::ResponseSupport()}),
        delegate_(
            OCMStrictProtocolMock(@protocol(OverlayRequestMediatorDelegate))) {
    scoped_feature_list_.InitWithFeatures({kIOSInfobarUIReboot},
                                          {kInfobarUIRebootOnlyiOS13});
    request_ = OverlayRequest::CreateWithConfig<TranslateModalRequestConfig>(
        &infobar_);
    callback_installer_.InstallCallbacks(request_.get());
    mediator_ = [[TranslateInfobarModalOverlayMediator alloc]
        initWithRequest:request_.get()];
    mediator_.delegate = delegate_;
  }

  ~TranslateInfobarModalOverlayMediatorTest() override {
    EXPECT_CALL(callback_receiver_, CompletionCallback(request_.get()));
    EXPECT_OCMOCK_VERIFY(delegate_);
  }

  FakeTranslateInfoBarDelegate& delegate() {
    return *static_cast<FakeTranslateInfoBarDelegate*>(infobar_.delegate());
  }

 protected:
  base::test::ScopedFeatureList scoped_feature_list_;
  FakeTranslateInfoBarDelegateFactory delegate_factory_;
  InfoBarIOS infobar_;
  MockOverlayRequestCallbackReceiver callback_receiver_;
  FakeOverlayRequestCallbackInstaller callback_installer_;
  std::unique_ptr<OverlayRequest> request_;
  id<OverlayRequestMediatorDelegate> delegate_ = nil;
  TranslateInfobarModalOverlayMediator* mediator_ = nil;
};

// Tests that a TranslateInfobarModalOverlayMediator correctly sets up its
// consumer.
TEST_F(TranslateInfobarModalOverlayMediatorTest, SetUpConsumer) {
  FakeInfobarTranslateModalConsumer* consumer =
      [[FakeInfobarTranslateModalConsumer alloc] init];
  mediator_.consumer = consumer;

  EXPECT_NSEQ(base::SysUTF16ToNSString(delegate().original_language_name()),
              consumer.sourceLanguage);
  EXPECT_NSEQ(base::SysUTF16ToNSString(delegate().target_language_name()),
              consumer.targetLanguage);
  EXPECT_TRUE(consumer.enableTranslateActionButton);
  EXPECT_FALSE(consumer.updateLanguageBeforeTranslate);
  EXPECT_FALSE(consumer.displayShowOriginalButton);
  EXPECT_TRUE(consumer.shouldAlwaysTranslate);
  EXPECT_TRUE(consumer.shouldDisplayNeverTranslateLanguageButton);
  EXPECT_TRUE(consumer.isTranslatableLanguage);
  EXPECT_TRUE(consumer.shouldDisplayNeverTranslateSiteButton);
  EXPECT_FALSE(consumer.isSiteBlacklisted);
}

// Tests that TranslateInfobarModalOverlayMediator calls RevertTranslation when
// its showOriginalLanguage API is called.
TEST_F(TranslateInfobarModalOverlayMediatorTest, ShowOriginalLanguage) {
  EXPECT_CALL(
      callback_receiver_,
      DispatchCallback(request_.get(), RevertTranslation::ResponseSupport()));
  OCMExpect([delegate_ stopOverlayForMediator:mediator_]);
  [mediator_ showOriginalLanguage];
}

// Tests that TranslateInfobarModalOverlayMediator calls UpdateLanguageInfo and
// InfobarModalMainActionResponse when its translateWithNewLanguages API is
// called, and didSelectSourceLanguageIndex and didSelectTargetLanguageIndex
// were called beforehand to change the source and target languages.
TEST_F(TranslateInfobarModalOverlayMediatorTest, UpdateLanguageInfo) {
  [mediator_ didSelectSourceLanguageIndex:69 withName:@"Portuguese"];
  [mediator_ didSelectTargetLanguageIndex:83 withName:@"Spanish"];
  request_->GetCallbackManager()->AddDispatchCallback(OverlayDispatchCallback(
      base::BindRepeating(^(OverlayResponse* response) {
        UpdateLanguageInfo* info = response->GetInfo<UpdateLanguageInfo>();
        ASSERT_TRUE(info);
        EXPECT_EQ(69, info->source_language_index());
        EXPECT_EQ(83, info->target_language_index());
      }),
      UpdateLanguageInfo::ResponseSupport()));
  EXPECT_CALL(
      callback_receiver_,
      DispatchCallback(request_.get(),
                       InfobarModalMainActionResponse::ResponseSupport()));
  OCMExpect([delegate_ stopOverlayForMediator:mediator_]);
  [mediator_ translateWithNewLanguages];
}

// Tests that TranslateInfobarModalOverlayMediator calls ToggleAlwaysTranslate
// and InfobarModalMainActionResponse when its alwaysTranslateSourceLanguage API
// is called.
TEST_F(TranslateInfobarModalOverlayMediatorTest,
       AlwaysTranslateSourceLanguage) {
  EXPECT_CALL(callback_receiver_,
              DispatchCallback(request_.get(),
                               ToggleAlwaysTranslate::ResponseSupport()));
  EXPECT_CALL(
      callback_receiver_,
      DispatchCallback(request_.get(),
                       InfobarModalMainActionResponse::ResponseSupport()));
  OCMExpect([delegate_ stopOverlayForMediator:mediator_]);
  [mediator_ alwaysTranslateSourceLanguage];
}

// Tests that TranslateInfobarModalOverlayMediator calls
// ToggleNeverTranslateSourceLanguage when its neverTranslateSourceLanguage API
// is called.
TEST_F(TranslateInfobarModalOverlayMediatorTest, NeverTranslateSourceLanguage) {
  EXPECT_CALL(
      callback_receiver_,
      DispatchCallback(request_.get(),
                       ToggleNeverTranslateSourceLanguage::ResponseSupport()));
  OCMExpect([delegate_ stopOverlayForMediator:mediator_]);
  [mediator_ neverTranslateSourceLanguage];
}

// Tests that TranslateInfobarModalOverlayMediator calls ToggleBlacklistSite
// when its neverTranslateSite API is called.
TEST_F(TranslateInfobarModalOverlayMediatorTest, NeverTranslateSite) {
  EXPECT_CALL(
      callback_receiver_,
      DispatchCallback(request_.get(), ToggleBlacklistSite::ResponseSupport()));
  OCMExpect([delegate_ stopOverlayForMediator:mediator_]);
  [mediator_ neverTranslateSite];
}
