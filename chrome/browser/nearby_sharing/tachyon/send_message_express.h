// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NEARBY_SHARING_TACHYON_SEND_MESSAGE_EXPRESS_H_
#define CHROME_BROWSER_NEARBY_SHARING_TACHYON_SEND_MESSAGE_EXPRESS_H_

#include <map>
#include <memory>
#include <string>

#include "base/callback.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"

namespace chrome_browser_nearby_sharing_tachyon {
class SendMessageExpressRequest;
}  // namespace chrome_browser_nearby_sharing_tachyon

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
}  // namespace network

class TokenFetcher;

// Sends messages using the Tachyon Express API over HTTP.
class SendMessageExpress {
 public:
  using SuccessCallback = base::OnceCallback<void(bool success)>;

  SendMessageExpress(
      TokenFetcher* token_fetcher,
      scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory);
  ~SendMessageExpress();

  void SendMessage(
      const chrome_browser_nearby_sharing_tachyon::SendMessageExpressRequest&
          request,
      SuccessCallback callback);

 private:
  void DoSendMessage(
      const chrome_browser_nearby_sharing_tachyon::SendMessageExpressRequest&
          request,
      SuccessCallback callback,
      const std::string& oauth_token);
  void OnSendMessageResponse(
      const std::string& message_id,
      std::unique_ptr<network::SimpleURLLoader> url_loader,
      SuccessCallback callback,
      std::unique_ptr<std::string> response_body);

  TokenFetcher* token_fetcher_;
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;
  base::WeakPtrFactory<SendMessageExpress> weak_ptr_factory_{this};
};

#endif  // CHROME_BROWSER_NEARBY_SHARING_TACHYON_SEND_MESSAGE_EXPRESS_H_
