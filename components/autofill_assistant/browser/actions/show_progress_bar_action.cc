// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/autofill_assistant/browser/actions/show_progress_bar_action.h"

#include <memory>
#include <utility>

#include "base/bind.h"
#include "base/callback.h"
#include "base/numerics/ranges.h"
#include "components/autofill_assistant/browser/actions/action_delegate.h"

namespace autofill_assistant {

ShowProgressBarAction::ShowProgressBarAction(ActionDelegate* delegate,
                                             const ActionProto& proto)
    : Action(delegate, proto) {
  DCHECK(proto_.has_show_progress_bar());
}

ShowProgressBarAction::~ShowProgressBarAction() = default;

void ShowProgressBarAction::InternalProcessAction(
    ProcessActionCallback callback) {
  if (proto_.show_progress_bar().has_message()) {
    // TODO(crbug.com/806868): Deprecate and remove message from this action and
    // use tell instead.
    delegate_->SetStatusMessage(proto_.show_progress_bar().message());
  }

  if (proto_.show_progress_bar().has_hide()) {
    delegate_->SetProgressVisible(!proto_.show_progress_bar().hide());
  }
  if (proto_.show_progress_bar().has_step_progress_bar_configuration()) {
    const auto& configuration =
        proto_.show_progress_bar().step_progress_bar_configuration();
    if (!configuration.step_icons().empty() &&
        configuration.step_icons().size() < 2) {
      EndAction(std::move(callback), INVALID_ACTION);
      return;
    }
    delegate_->SetStepProgressBarConfiguration(configuration);
  }

  switch (proto_.show_progress_bar().progress_indicator_case()) {
    case ShowProgressBarProto::ProgressIndicatorCase::kProgress: {
      int progress =
          base::ClampToRange(proto_.show_progress_bar().progress(), 0, 100);
      delegate_->SetProgress(progress);
      break;
    }
    case ShowProgressBarProto::ProgressIndicatorCase::kActiveStep: {
      int active_step = proto_.show_progress_bar().active_step();
      if (active_step < 0) {
        EndAction(std::move(callback), INVALID_ACTION);
        return;
      }

      delegate_->SetProgressActiveStep(active_step);
      break;
    }
    default:
      // Ignore.
      break;
  }

  EndAction(std::move(callback), ACTION_APPLIED);
}

void ShowProgressBarAction::EndAction(ProcessActionCallback callback,
                                      ProcessedActionStatusProto status) {
  UpdateProcessedAction(status);
  std::move(callback).Run(std::move(processed_action_proto_));
}

}  // namespace autofill_assistant
