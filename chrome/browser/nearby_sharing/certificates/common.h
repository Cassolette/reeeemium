// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NEARBY_SHARING_CERTIFICATES_COMMON_H_
#define CHROME_BROWSER_NEARBY_SHARING_CERTIFICATES_COMMON_H_

#include <memory>
#include <vector>

#include "base/containers/span.h"
#include "base/time/time.h"

namespace crypto {
class Encryptor;
class SymmetricKey;
}  // namespace crypto

// Returns true if the |current_time| exceeds |not_after| by more than the
// public certificate clock-skew tolerance if applicable.
bool IsNearbyShareCertificateExpired(base::Time current_time,
                                     base::Time not_after,
                                     bool use_public_certificate_tolerance);

// Returns true if the |current_time| is in the interval
// [|not_before| - tolerance, |not_after| + tolerance), where a clock-skew
// tolerance is only non-zero if |use_public_certificate_tolerance| is true.
bool IsNearbyShareCertificateWithinValidityPeriod(
    base::Time current_time,
    base::Time not_before,
    base::Time not_after,
    bool use_public_certificate_tolerance);

// Uses HKDF to generate a new key of length |new_num_bytes| from |key|. To
// conform with the GmsCore implementation, trivial salt and info are used.
std::vector<uint8_t> DeriveNearbyShareKey(base::span<const uint8_t> key,
                                          size_t new_num_bytes);

// Creates a CTR encryptor used for metadata key encryption/decryption.
std::unique_ptr<crypto::Encryptor> CreateNearbyShareCtrEncryptor(
    const crypto::SymmetricKey* secret_key,
    base::span<const uint8_t> salt);

#endif  // CHROME_BROWSER_NEARBY_SHARING_CERTIFICATES_COMMON_H_
