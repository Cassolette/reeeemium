// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/frame/dactyloscoper.h"

#include "third_party/blink/public/common/privacy_budget/identifiability_metric_builder.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"

namespace blink {

Dactyloscoper::Dactyloscoper() = default;

void Dactyloscoper::Record(WebFeature feature) {
  // TODO(mkwst): This is a stub. We'll pull in more interesting functionality
  // here over time.
}

// static
void Dactyloscoper::Record(ExecutionContext* context, WebFeature feature) {
  // TODO: Workers.
  if (!context)
    return;
  if (auto* window = DynamicTo<LocalDOMWindow>(context)) {
    if (auto* frame = window->GetFrame())
      frame->Loader().GetDocumentLoader()->GetDactyloscoper().Record(feature);
  }
}

// static
void Dactyloscoper::RecordDirectSurface(ExecutionContext* context,
                                        WebFeature feature,
                                        unsigned value) {
  if (!context)
    return;
  auto* window = DynamicTo<LocalDOMWindow>(context);
  if (!window)
    return;
  if (Document* document = window->document()) {
    IdentifiabilityMetricBuilder(document->UkmSourceID())
        .SetWebfeature(feature, value)
        .Record(document->UkmRecorder());
  }
}

}  // namespace blink
