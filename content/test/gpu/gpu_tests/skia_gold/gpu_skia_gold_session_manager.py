# Copyright 2020 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""GPU impl of //testing/skia_gold_common/skia_gold_session_manager.py."""

from gpu_tests import path_util
from gpu_tests.skia_gold import gpu_skia_gold_session

path_util.AddDirToPathIfNeeded(path_util.GetChromiumSrcDir(), 'build')
from skia_gold_common import skia_gold_session_manager as sgsm


class GpuSkiaGoldSessionManager(sgsm.SkiaGoldSessionManager):
  @staticmethod
  def _GetDefaultInstance():
    return 'chrome-gpu'

  @staticmethod
  def _GetSessionClass():
    return gpu_skia_gold_session.GpuSkiaGoldSession
