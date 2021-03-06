// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/xr/xr_anchor.h"
#include "third_party/blink/renderer/modules/xr/xr_object_space.h"
#include "third_party/blink/renderer/modules/xr/xr_session.h"
#include "third_party/blink/renderer/modules/xr/xr_system.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"

namespace {

constexpr char kAnchorAlreadyDeleted[] =
    "Unable to access anchor properties, the anchor was already deleted.";

}

namespace blink {

XRAnchor::XRAnchor(uint64_t id,
                   XRSession* session,
                   const device::mojom::blink::XRAnchorData& anchor_data)
    : id_(id),
      is_deleted_(false),
      session_(session),
      mojo_from_anchor_(anchor_data.mojo_from_anchor) {}

void XRAnchor::Update(const device::mojom::blink::XRAnchorData& anchor_data) {
  if (is_deleted_) {
    return;
  }

  mojo_from_anchor_ = anchor_data.mojo_from_anchor;
}

uint64_t XRAnchor::id() const {
  return id_;
}

XRSpace* XRAnchor::anchorSpace(ExceptionState& exception_state) const {
  if (is_deleted_) {
    exception_state.ThrowDOMException(DOMExceptionCode::kInvalidStateError,
                                      kAnchorAlreadyDeleted);
    return nullptr;
  }

  if (!anchor_space_) {
    anchor_space_ =
        MakeGarbageCollected<XRObjectSpace<XRAnchor>>(session_, this);
  }

  return anchor_space_;
}

base::Optional<TransformationMatrix> XRAnchor::MojoFromObject() const {
  if (!mojo_from_anchor_) {
    return base::nullopt;
  }

  return mojo_from_anchor_->ToTransform().matrix();
}

void XRAnchor::Delete() {
  if (!is_deleted_) {
    session_->xr()->xrEnvironmentProviderRemote()->DetachAnchor(id_);
    mojo_from_anchor_ = base::nullopt;
    anchor_space_ = nullptr;
  }

  is_deleted_ = true;
}

void XRAnchor::Trace(Visitor* visitor) const {
  visitor->Trace(session_);
  visitor->Trace(anchor_space_);
  ScriptWrappable::Trace(visitor);
}

}  // namespace blink
