// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/test/test_network_service_client.h"

#include <utility>

#include "base/optional.h"
#include "base/task/post_task.h"
#include "base/unguessable_token.h"

namespace network {

TestNetworkServiceClient::TestNetworkServiceClient() : receiver_(nullptr) {}

TestNetworkServiceClient::TestNetworkServiceClient(
    mojo::PendingReceiver<mojom::NetworkServiceClient> receiver)
    : receiver_(this, std::move(receiver)) {}

TestNetworkServiceClient::~TestNetworkServiceClient() {}

void TestNetworkServiceClient::OnLoadingStateUpdate(
    std::vector<mojom::LoadInfoPtr> infos,
    OnLoadingStateUpdateCallback callback) {}

void TestNetworkServiceClient::OnDataUseUpdate(
    int32_t network_traffic_annotation_id_hash,
    int64_t recv_bytes,
    int64_t sent_bytes) {}

void TestNetworkServiceClient::OnRawRequest(
    int32_t process_id,
    int32_t routing_id,
    const std::string& devtools_request_id,
    const net::CookieAccessResultList& cookies_with_access_result,
    std::vector<network::mojom::HttpRawHeaderPairPtr> headers) {}

void TestNetworkServiceClient::OnRawResponse(
    int32_t process_id,
    int32_t routing_id,
    const std::string& devtools_request_id,
    const net::CookieAndLineStatusList& cookies_with_status,
    std::vector<network::mojom::HttpRawHeaderPairPtr> headers,
    const base::Optional<std::string>& raw_response_headers) {}

void TestNetworkServiceClient::OnCorsPreflightRequest(
    int32_t process_id,
    int32_t routing_id,
    const base::UnguessableToken& devtools_request_id,
    const network::ResourceRequest& request,
    const GURL& initiator_url) {}

void TestNetworkServiceClient::OnCorsPreflightResponse(
    int32_t process_id,
    int32_t routing_id,
    const base::UnguessableToken& devtools_request_id,
    const GURL& url,
    network::mojom::URLResponseHeadPtr head) {}

void TestNetworkServiceClient::OnCorsPreflightRequestCompleted(
    int32_t process_id,
    int32_t routing_id,
    const base::UnguessableToken& devtool_request_id,
    const network::URLLoaderCompletionStatus& status) {}

void TestNetworkServiceClient::LogCrossOriginFetchFromContentScript3(
    const std::string& isolated_world_host) {}

}  // namespace network
