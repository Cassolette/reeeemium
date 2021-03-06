// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBLAYER_BROWSER_BROWSING_DATA_REMOVER_DELEGATE_H_
#define WEBLAYER_BROWSER_BROWSING_DATA_REMOVER_DELEGATE_H_

#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/browsing_data_remover.h"
#include "content/public/browser/browsing_data_remover_delegate.h"

namespace content {
class BrowserContext;
}

namespace weblayer {

class BrowsingDataRemoverDelegate : public content::BrowsingDataRemoverDelegate,
                                    public KeyedService {
 public:
  // This is an extension of content::BrowsingDataRemover::RemoveDataMask which
  // includes all datatypes therefrom and adds additional WebLayer-specific
  // ones.
  enum DataType : uint64_t {
    // Embedder can start adding datatypes after the last platform datatype.
    DATA_TYPE_EMBEDDER_BEGIN =
        content::BrowsingDataRemover::DATA_TYPE_CONTENT_END << 1,

    // WebLayer-specific datatypes.
    DATA_TYPE_ISOLATED_ORIGINS = DATA_TYPE_EMBEDDER_BEGIN,
  };

  explicit BrowsingDataRemoverDelegate(
      content::BrowserContext* browser_context);

  BrowsingDataRemoverDelegate(const BrowsingDataRemoverDelegate&) = delete;
  BrowsingDataRemoverDelegate& operator=(const BrowsingDataRemoverDelegate&) =
      delete;

  // content::BrowsingDataRemoverDelegate:
  EmbedderOriginTypeMatcher GetOriginTypeMatcher() override;
  bool MayRemoveDownloadHistory() override;
  std::vector<std::string> GetDomainsForDeferredCookieDeletion(
      uint64_t remove_mask) override;
  void RemoveEmbedderData(const base::Time& delete_begin,
                          const base::Time& delete_end,
                          uint64_t remove_mask,
                          content::BrowsingDataFilterBuilder* filter_builder,
                          uint64_t origin_type_mask,
                          base::OnceClosure callback) override;

 private:
  content::BrowserContext* browser_context_ = nullptr;
};

}  // namespace weblayer

#endif  // WEBLAYER_BROWSER_BROWSING_DATA_REMOVER_DELEGATE_H_
