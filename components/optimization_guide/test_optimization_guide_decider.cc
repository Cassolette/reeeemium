// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/optimization_guide/test_optimization_guide_decider.h"

#include "content/public/browser/navigation_handle.h"

namespace optimization_guide {

TestOptimizationGuideDecider::TestOptimizationGuideDecider() = default;
TestOptimizationGuideDecider::~TestOptimizationGuideDecider() = default;

void TestOptimizationGuideDecider::RegisterOptimizationTypesAndTargets(
    const std::vector<proto::OptimizationType>& optimization_types,
    const std::vector<proto::OptimizationTarget>& optimization_targets) {}

OptimizationGuideDecision TestOptimizationGuideDecider::ShouldTargetNavigation(
    content::NavigationHandle* navigation_handle,
    proto::OptimizationTarget optimization_target) {
  return OptimizationGuideDecision::kFalse;
}

void TestOptimizationGuideDecider::CanApplyOptimizationAsync(
    content::NavigationHandle* navigation_handle,
    proto::OptimizationType optimization_type,
    OptimizationGuideDecisionCallback callback) {
  std::move(callback).Run(OptimizationGuideDecision::kFalse,
                          /*optimization_metadata=*/{});
}

OptimizationGuideDecision TestOptimizationGuideDecider::CanApplyOptimization(
    const GURL& url,
    proto::OptimizationType optimization_type,
    OptimizationMetadata* optimization_metadata) {
  return OptimizationGuideDecision::kFalse;
}

}  // namespace optimization_guide
