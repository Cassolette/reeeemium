// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_PUBLIC_CPP_ASSISTANT_CONTROLLER_ASSISTANT_UI_CONTROLLER_H_
#define ASH_PUBLIC_CPP_ASSISTANT_CONTROLLER_ASSISTANT_UI_CONTROLLER_H_

#include "ash/public/cpp/ash_public_export.h"
#include "base/optional.h"

namespace chromeos {
namespace assistant {
enum class AssistantEntryPoint;
enum class AssistantExitPoint;
}  // namespace assistant
}  // namespace chromeos

namespace ash {

class AssistantUiModel;

// The interface for the Assistant controller in charge of UI.
class ASH_PUBLIC_EXPORT AssistantUiController {
 public:
  // Returns the singleton instance owned by AssistantController.
  static AssistantUiController* Get();

  // Returns a pointer to the underlying model.
  virtual const AssistantUiModel* GetModel() const = 0;

  // Invoke to show/close/toggle Assistant UI.
  virtual void ShowUi(chromeos::assistant::AssistantEntryPoint) = 0;
  virtual void CloseUi(chromeos::assistant::AssistantExitPoint) = 0;
  virtual void ToggleUi(
      base::Optional<chromeos::assistant::AssistantEntryPoint>,
      base::Optional<chromeos::assistant::AssistantExitPoint>) = 0;

 protected:
  AssistantUiController();
  virtual ~AssistantUiController();
};

}  // namespace ash

#endif  // ASH_PUBLIC_CPP_ASSISTANT_CONTROLLER_ASSISTANT_UI_CONTROLLER_H_
