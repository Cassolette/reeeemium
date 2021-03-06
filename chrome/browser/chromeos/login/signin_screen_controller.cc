// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/signin_screen_controller.h"

#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/chromeos/login/lock/screen_locker.h"
#include "chrome/browser/chromeos/login/screens/chrome_user_selection_screen.h"
#include "chrome/browser/chromeos/login/ui/views/user_board_view.h"
#include "chrome/browser/ui/webui/chromeos/login/oobe_ui.h"
#include "chrome/browser/ui/webui/chromeos/login/signin_screen_handler.h"
#include "chrome/browser/ui/webui/chromeos/login/user_board_screen_handler.h"

namespace chromeos {

SignInScreenController* SignInScreenController::instance_ = nullptr;

SignInScreenController::SignInScreenController(OobeUI* oobe_ui) {
  DCHECK(!instance_);
  instance_ = this;

  std::string display_type = oobe_ui->display_type();
  user_selection_screen_.reset(new ChromeUserSelectionScreen(display_type));

  user_board_view_ = oobe_ui->GetView<UserBoardScreenHandler>()->GetWeakPtr();
  user_selection_screen_->SetView(user_board_view_.get());
  // TODO(jdufault): Bind and Unbind should be controlled by either the
  // Model/View which are then each responsible for automatically unbinding the
  // other associated View/Model instance. Then we can eliminate this exposed
  // WeakPtr logic. See crbug.com/685287.
  user_board_view_->Bind(user_selection_screen_.get());

  user_manager::UserManager::Get()->AddObserver(this);
}

SignInScreenController::~SignInScreenController() {
  if (user_board_view_)
    user_board_view_->Unbind();

  user_manager::UserManager::Get()->RemoveObserver(this);
  instance_ = nullptr;
}

void SignInScreenController::Init(const user_manager::UserList& users) {
  // TODO(antrim) : This dependency should be inverted, screen should ask about
  // users.
  user_selection_screen_->Init(users);
}

void SignInScreenController::OnSigninScreenReady() {
  user_selection_screen_->InitEasyUnlock();
}

void SignInScreenController::OnUserImageChanged(
    const user_manager::User& user) {
  user_selection_screen_->OnUserImageChanged(user);
}

void SignInScreenController::SendUserList() {
  user_selection_screen_->HandleGetUsers();
}

void SignInScreenController::CheckUserStatus(const AccountId& account_id) {
  user_selection_screen_->CheckUserStatus(account_id);
}

void SignInScreenController::SetWebUIHandler(
    LoginDisplayWebUIHandler* webui_handler) {
  webui_handler_ = webui_handler;
  user_selection_screen_->SetHandler(webui_handler_);
}

}  // namespace chromeos
