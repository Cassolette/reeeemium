// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/worker_host/dedicated_worker_service_impl.h"

#include "base/stl_util.h"

namespace content {

DedicatedWorkerServiceImpl::DedicatedWorkerInfo::DedicatedWorkerInfo(
    int worker_process_id,
    GlobalFrameRoutingId ancestor_render_frame_host_id)
    : worker_process_id(worker_process_id),
      ancestor_render_frame_host_id(ancestor_render_frame_host_id) {}

DedicatedWorkerServiceImpl::DedicatedWorkerInfo::DedicatedWorkerInfo(
    const DedicatedWorkerInfo& info) = default;
DedicatedWorkerServiceImpl::DedicatedWorkerInfo&
DedicatedWorkerServiceImpl::DedicatedWorkerInfo::operator=(
    const DedicatedWorkerInfo& info) = default;

DedicatedWorkerServiceImpl::DedicatedWorkerInfo::~DedicatedWorkerInfo() =
    default;

DedicatedWorkerServiceImpl::DedicatedWorkerServiceImpl() = default;

DedicatedWorkerServiceImpl::~DedicatedWorkerServiceImpl() = default;

void DedicatedWorkerServiceImpl::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void DedicatedWorkerServiceImpl::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void DedicatedWorkerServiceImpl::EnumerateDedicatedWorkers(Observer* observer) {
  for (const auto& kv : dedicated_worker_infos_) {
    DedicatedWorkerId dedicated_worker_id = kv.first;
    const DedicatedWorkerInfo& dedicated_worker_info = kv.second;

    observer->OnWorkerCreated(
        dedicated_worker_id, dedicated_worker_info.worker_process_id,
        dedicated_worker_info.ancestor_render_frame_host_id);
    if (dedicated_worker_info.final_response_url) {
      observer->OnFinalResponseURLDetermined(
          dedicated_worker_id, *dedicated_worker_info.final_response_url);
    }
  }
}

DedicatedWorkerId DedicatedWorkerServiceImpl::GenerateNextDedicatedWorkerId() {
  return dedicated_worker_id_generator_.GenerateNextId();
}

void DedicatedWorkerServiceImpl::NotifyWorkerCreated(
    DedicatedWorkerId dedicated_worker_id,
    int worker_process_id,
    GlobalFrameRoutingId ancestor_render_frame_host_id) {
  bool inserted =
      dedicated_worker_infos_
          .emplace(dedicated_worker_id,
                   DedicatedWorkerInfo(worker_process_id,
                                       ancestor_render_frame_host_id))
          .second;
  DCHECK(inserted);

  for (Observer& observer : observers_) {
    observer.OnWorkerCreated(dedicated_worker_id, worker_process_id,
                             ancestor_render_frame_host_id);
  }
}

void DedicatedWorkerServiceImpl::NotifyBeforeWorkerDestroyed(
    DedicatedWorkerId dedicated_worker_id,
    GlobalFrameRoutingId ancestor_render_frame_host_id) {
  size_t removed = dedicated_worker_infos_.erase(dedicated_worker_id);
  DCHECK_EQ(removed, 1u);

  for (Observer& observer : observers_) {
    observer.OnBeforeWorkerDestroyed(dedicated_worker_id,
                                     ancestor_render_frame_host_id);
  }
}

void DedicatedWorkerServiceImpl::NotifyWorkerFinalResponseURLDetermined(
    DedicatedWorkerId dedicated_worker_id,
    const GURL& url) {
  auto it = dedicated_worker_infos_.find(dedicated_worker_id);
  DCHECK(it != dedicated_worker_infos_.end());

  it->second.final_response_url = url;

  for (Observer& observer : observers_)
    observer.OnFinalResponseURLDetermined(dedicated_worker_id, url);
}

}  // namespace content
