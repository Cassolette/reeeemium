// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/autofill_assistant/browser/trigger_context.h"

#include "components/autofill_assistant/browser/service.pb.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace autofill_assistant {
namespace {

using ::testing::IsEmpty;
using ::testing::SizeIs;

TEST(TriggerContextTest, Empty) {
  auto empty = TriggerContext::CreateEmpty();
  ASSERT_TRUE(empty);

  google::protobuf::RepeatedPtrField<ScriptParameterProto> parameters_proto;
  empty->AddParameters(&parameters_proto);
  EXPECT_THAT(parameters_proto, IsEmpty());

  EXPECT_EQ("", empty->experiment_ids());
}

TEST(TriggerContextTest, Create) {
  std::map<std::string, std::string> parameters;
  parameters["a"] = "b";
  parameters["c"] = "d";
  auto context = TriggerContext::Create(parameters, "exps");
  ASSERT_TRUE(context);

  google::protobuf::RepeatedPtrField<ScriptParameterProto> parameters_proto;
  context->AddParameters(&parameters_proto);
  EXPECT_THAT(parameters_proto, SizeIs(2));

  EXPECT_EQ("a", parameters_proto.Get(0).name());
  EXPECT_EQ("b", parameters_proto.Get(0).value());
  EXPECT_EQ("c", parameters_proto.Get(1).name());
  EXPECT_EQ("d", parameters_proto.Get(1).value());

  EXPECT_EQ("b", context->GetParameter("a").value_or(""));
  EXPECT_EQ("d", context->GetParameter("c").value_or(""));
  EXPECT_FALSE(context->GetParameter("not_found"));

  EXPECT_EQ("exps", context->experiment_ids());
}

TEST(TriggerContextTest, Merge) {
  std::map<std::string, std::string> parameters;
  parameters["a"] = "b";
  auto context1 = TriggerContext::Create(parameters, "exp1");

  parameters.clear();
  parameters["c"] = "d";
  auto context2 = TriggerContext::Create(parameters, "exp2");

  // Adding empty to make sure empty contexts are properly skipped
  auto empty = TriggerContext::CreateEmpty();

  auto merged = TriggerContext::Merge(
      {empty.get(), context1.get(), context2.get(), empty.get()});

  ASSERT_TRUE(merged);

  google::protobuf::RepeatedPtrField<ScriptParameterProto> parameters_proto;
  merged->AddParameters(&parameters_proto);
  EXPECT_THAT(parameters_proto, SizeIs(2));

  EXPECT_EQ("a", parameters_proto.Get(0).name());
  EXPECT_EQ("b", parameters_proto.Get(0).value());
  EXPECT_EQ("c", parameters_proto.Get(1).name());
  EXPECT_EQ("d", parameters_proto.Get(1).value());

  EXPECT_EQ("b", merged->GetParameter("a").value_or(""));
  EXPECT_EQ("d", merged->GetParameter("c").value_or(""));
  EXPECT_FALSE(merged->GetParameter("not_found"));

  EXPECT_EQ("exp1,exp2", merged->experiment_ids());
}

TEST(TriggerContextTest, CCT) {
  TriggerContextImpl context;

  EXPECT_FALSE(context.is_cct());
  context.SetCCT(true);
  EXPECT_TRUE(context.is_cct());
}

TEST(TriggerContextTest, MergeCCT) {
  auto empty = TriggerContext::CreateEmpty();

  auto all_empty = TriggerContext::Merge({empty.get(), empty.get()});
  EXPECT_FALSE(all_empty->is_cct());

  TriggerContextImpl cct_context;
  cct_context.SetCCT(true);
  auto one_with_cct =
      TriggerContext::Merge({empty.get(), &cct_context, empty.get()});

  EXPECT_TRUE(one_with_cct->is_cct());
}

TEST(TriggerContextTest, OnboardingShown) {
  TriggerContextImpl context;

  EXPECT_FALSE(context.is_onboarding_shown());
  context.SetOnboardingShown(true);
  EXPECT_TRUE(context.is_onboarding_shown());
}

TEST(TriggerContextTest, MergeOnboardingShown) {
  auto empty = TriggerContext::CreateEmpty();

  auto all_empty = TriggerContext::Merge({empty.get(), empty.get()});
  EXPECT_FALSE(all_empty->is_onboarding_shown());

  TriggerContextImpl onboarding_context;
  onboarding_context.SetOnboardingShown(true);
  auto one_with_onboarding =
      TriggerContext::Merge({empty.get(), &onboarding_context, empty.get()});

  EXPECT_TRUE(one_with_onboarding->is_onboarding_shown());
}

TEST(TriggerContextTest, DirectAction) {
  TriggerContextImpl context;

  EXPECT_FALSE(context.is_direct_action());
  context.SetDirectAction(true);
  EXPECT_TRUE(context.is_direct_action());
}

TEST(TriggerContextTest, MergeDirectAction) {
  auto empty = TriggerContext::CreateEmpty();

  auto all_empty = TriggerContext::Merge({empty.get(), empty.get()});
  EXPECT_FALSE(all_empty->is_direct_action());

  TriggerContextImpl direct_action_context;
  direct_action_context.SetDirectAction(true);
  auto one_direct_action =
      TriggerContext::Merge({empty.get(), &direct_action_context, empty.get()});

  EXPECT_TRUE(one_direct_action->is_direct_action());
}

TEST(TriggerContextTest, MergeAccountsMatchingStatusTest) {
  auto empty = TriggerContext::CreateEmpty();

  auto all_empty = TriggerContext::Merge({empty.get(), empty.get()});
  EXPECT_EQ(all_empty->get_caller_account_hash(), "");

  TriggerContextImpl context_matching;
  context_matching.SetCallerAccountHash("accountsha");
  auto one_with_accounts_matching =
      TriggerContext::Merge({empty.get(), &context_matching, empty.get()});

  EXPECT_EQ(one_with_accounts_matching->get_caller_account_hash(),
            "accountsha");
}

TEST(TriggerContextTest, HasExperimentId) {
  std::map<std::string, std::string> parameters;

  auto context = TriggerContext::Create(parameters, "1,2,3");
  ASSERT_TRUE(context);

  EXPECT_TRUE(context->HasExperimentId("2"));
  EXPECT_FALSE(context->HasExperimentId("4"));

  auto other_context = TriggerContext::Create(parameters, "4,5,6");
  ASSERT_TRUE(other_context);

  EXPECT_TRUE(other_context->HasExperimentId("4"));
  EXPECT_FALSE(other_context->HasExperimentId("2"));

  auto merged = TriggerContext::Merge({context.get(), other_context.get()});
  ASSERT_TRUE(merged);

  EXPECT_TRUE(merged->HasExperimentId("2"));
  EXPECT_TRUE(merged->HasExperimentId("4"));
  EXPECT_FALSE(merged->HasExperimentId("7"));

  // Double commas should not allow empty element to match.
  auto double_comma = TriggerContext::Create(parameters, "1,,2");
  EXPECT_TRUE(double_comma->HasExperimentId("2"));
  EXPECT_FALSE(double_comma->HasExperimentId(""));

  // Empty context should not aloow empty element to match.
  auto empty = TriggerContext::Create(parameters, "");
  EXPECT_FALSE(empty->HasExperimentId(""));

  // Lone comma does not create empty elements.
  auto lone_comma = TriggerContext::Create(parameters, ",");
  EXPECT_FALSE(lone_comma->HasExperimentId(""));

  // Single element should match.
  auto single_element = TriggerContext::Create(parameters, "1");
  EXPECT_TRUE(single_element->HasExperimentId("1"));
}

}  // namespace
}  // namespace autofill_assistant
