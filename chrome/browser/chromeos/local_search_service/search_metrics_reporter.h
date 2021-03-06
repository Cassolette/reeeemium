// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_LOCAL_SEARCH_SERVICE_SEARCH_METRICS_REPORTER_H_
#define CHROME_BROWSER_CHROMEOS_LOCAL_SEARCH_SERVICE_SEARCH_METRICS_REPORTER_H_

#include <array>
#include <memory>
#include <string>

#include "base/macros.h"
#include "base/optional.h"
#include "base/timer/timer.h"
#include "chrome/browser/chromeos/local_search_service/shared_structs.h"
#include "components/metrics/daily_event.h"

class PrefRegistrySimple;
class PrefService;

namespace local_search_service {

// SearchMetricsReporter logs daily search requests to UMA.
class SearchMetricsReporter {
 public:
  static constexpr int kNumberIndexIds =
      static_cast<int>(IndexId::kMaxValue) + 1;

  // A histogram recorded in UMA, showing reasons why daily metrics are
  // reported.
  static constexpr char kDailyEventIntervalName[] =
      "LocalSearchService.MetricsDailyEventInterval";

  // Histogram names of daily counts, one for each IndexId.
  static constexpr char kCrosSettingsName[] =
      "LocalSearchService.CrosSettings.DailySearch";

  // Registers prefs used by SearchMetricsReporter in |registry|.
  static void RegisterLocalStatePrefs(PrefRegistrySimple* registry);

  // RegisterLocalStatePrefs() must be called before instantiating this class.
  explicit SearchMetricsReporter(PrefService* local_state_pref_service);
  ~SearchMetricsReporter();

  SearchMetricsReporter(const SearchMetricsReporter&) = delete;
  SearchMetricsReporter& operator=(const SearchMetricsReporter&) = delete;

  // Sets |index_id_|.
  void SetIndexId(IndexId index_id);

  // Increments number of searches for |index_id_|. Should only
  // be called after |SetIndexId| is called.
  void OnSearchPerformed();

  // Calls ReportDailyMetrics directly.
  void ReportDailyMetricsForTesting(metrics::DailyEvent::IntervalType type);

 private:
  class DailyEventObserver;

  // Called by DailyEventObserver whenever a day has elapsed according to
  // |daily_event_|.
  void ReportDailyMetrics(metrics::DailyEvent::IntervalType type);

  // Used as an index into |daily_counts_| for counting searches.
  base::Optional<IndexId> index_id_;

  PrefService* pref_service_;  // Not owned.

  std::unique_ptr<metrics::DailyEvent> daily_event_;

  // Instructs |daily_event_| to check if a day has passed.
  base::RepeatingTimer timer_;

  // Daily count for each index id. Ordered by IndexId values.
  // Initial values will be loaded from prefs service.
  std::array<int, kNumberIndexIds> daily_counts_;
};

}  // namespace local_search_service

#endif  // CHROME_BROWSER_CHROMEOS_LOCAL_SEARCH_SERVICE_SEARCH_METRICS_REPORTER_H_
