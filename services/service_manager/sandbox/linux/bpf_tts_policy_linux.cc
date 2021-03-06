// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/service_manager/sandbox/linux/bpf_tts_policy_linux.h"

#include <sys/socket.h>

#include "sandbox/linux/bpf_dsl/bpf_dsl.h"
#include "sandbox/linux/seccomp-bpf-helpers/syscall_parameters_restrictions.h"
#include "sandbox/linux/syscall_broker/broker_process.h"
#include "sandbox/linux/system_headers/linux_syscalls.h"
#include "services/service_manager/sandbox/linux/sandbox_linux.h"

using sandbox::bpf_dsl::Allow;
using sandbox::bpf_dsl::ResultExpr;
using sandbox::bpf_dsl::Trap;
using sandbox::syscall_broker::BrokerProcess;

namespace service_manager {

TtsProcessPolicy::TtsProcessPolicy() {}

TtsProcessPolicy::~TtsProcessPolicy() {}

ResultExpr TtsProcessPolicy::EvaluateSyscall(int sysno) const {
  auto* broker_process = SandboxLinux::GetInstance()->broker_process();
  if (broker_process->IsSyscallAllowed(sysno))
    return Trap(BrokerProcess::SIGSYS_Handler, broker_process);

  return BPFBasePolicy::EvaluateSyscall(sysno);
}

}  // namespace service_manager
