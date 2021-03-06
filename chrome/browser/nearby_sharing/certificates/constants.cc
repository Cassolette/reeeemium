// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/nearby_sharing/certificates/constants.h"

const base::TimeDelta kNearbyShareCertificateValidityPeriod =
    base::TimeDelta::FromDays(3);
const base::TimeDelta kNearbyShareMaxPrivateCertificateValidityBoundOffset =
    base::TimeDelta::FromHours(2);
const base::TimeDelta
    kNearbySharePublicCertificateValidityBoundOffsetTolerance =
        base::TimeDelta::FromMinutes(30);
const size_t kNearbyShareNumPrivateCertificates = 3;
const size_t kNearbyShareNumBytesAuthenticationTokenHash = 6;
const size_t kNearbyShareNumBytesAesGcmKey = 32;
const size_t kNearbyShareNumBytesAesGcmIv = 12;
const size_t kNearbyShareNumBytesAesCtrIv = 16;
const size_t kNearbyShareNumBytesSecretKey = 32;
const size_t kNearbyShareNumBytesMetadataEncryptionKey = 14;
const size_t kNearbyShareNumBytesMetadataEncryptionKeySalt = 2;
const size_t kNearbyShareNumBytesMetadataEncryptionKeyTag = 32;
const size_t kNearbyShareNumBytesCertificateId = 32;
const size_t kNearbyShareMaxNumMetadataEncryptionKeySalts = 32768;
const size_t kNearbyShareMaxNumMetadataEncryptionKeySaltGenerationRetries = 128;
const char kNearbyShareSenderVerificationPrefix = 0x01;
const char kNearbyShareReceiverVerificationPrefix = 0x02;
