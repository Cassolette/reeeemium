// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/audio_renderer_mixer.h"

#include <cmath>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/check_op.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/base/audio_renderer_mixer_input.h"
#include "media/base/audio_timestamp_helper.h"
#include "media/base/media_switches.h"
#include "media/base/silent_sink_suspender.h"

namespace media {

enum { kPauseDelaySeconds = 10 };

AudioRendererMixer::AudioRendererMixer(const AudioParameters& output_params,
                                       scoped_refptr<AudioRendererSink> sink)
    : output_params_(output_params),
      audio_sink_(std::move(sink)),
      master_converter_(output_params, output_params, true),
      pause_delay_(base::TimeDelta::FromSeconds(kPauseDelaySeconds)),
      last_play_time_(base::TimeTicks::Now()),
      // Initialize |playing_| to true since Start() results in an auto-play.
      playing_(true) {
  DCHECK(audio_sink_);

  // If enabled we will disable the real audio output stream for muted/silent
  // playbacks after some time elapses.
  RenderCallback* callback = this;
  if (base::FeatureList::IsEnabled(media::kSuspendMutedAudio)) {
    // We use slightly more than |pause_delay_| time before suspending the sink
    // to ensure that we just Pause() entirely instead of using a fake sink when
    // possible.
    muted_suspender_.reset(new SilentSinkSuspender(
        this, pause_delay_ + base::TimeDelta::FromMilliseconds(500),
        output_params, audio_sink_, GetSuspenderTaskRunner()));
    callback = muted_suspender_.get();
  }

  audio_sink_->Initialize(output_params, callback);
  audio_sink_->Start();
}

AudioRendererMixer::~AudioRendererMixer() {
  // AudioRendererSink must be stopped before mixer is destructed.
  audio_sink_->Stop();
  muted_suspender_.reset();

  // Ensure that all mixer inputs have removed themselves prior to destruction.
  DCHECK(master_converter_.empty());
  DCHECK(converters_.empty());
  DCHECK(error_callbacks_.empty());
}

void AudioRendererMixer::AddMixerInput(const AudioParameters& input_params,
                                       AudioConverter::InputCallback* input) {
  base::AutoLock auto_lock(lock_);
  if (!playing_) {
    playing_ = true;
    last_play_time_ = base::TimeTicks::Now();
    audio_sink_->Play();
  }

  int input_sample_rate = input_params.sample_rate();
  if (is_master_sample_rate(input_sample_rate)) {
    master_converter_.AddInput(input);
  } else {
    auto converter = converters_.find(input_sample_rate);
    if (converter == converters_.end()) {
      std::pair<AudioConvertersMap::iterator, bool> result =
          converters_.insert(std::make_pair(
              input_sample_rate, std::make_unique<LoopbackAudioConverter>(
                                     // We expect all InputCallbacks to be
                                     // capable of handling arbitrary buffer
                                     // size requests, disabling FIFO.
                                     input_params, output_params_, true)));
      converter = result.first;

      // Add newly-created resampler as an input to the master mixer.
      master_converter_.AddInput(converter->second.get());
    }
    converter->second->AddInput(input);
  }
}

void AudioRendererMixer::RemoveMixerInput(
    const AudioParameters& input_params,
    AudioConverter::InputCallback* input) {
  base::AutoLock auto_lock(lock_);

  int input_sample_rate = input_params.sample_rate();
  if (is_master_sample_rate(input_sample_rate)) {
    master_converter_.RemoveInput(input);
  } else {
    auto converter = converters_.find(input_sample_rate);
    DCHECK(converter != converters_.end());
    converter->second->RemoveInput(input);
    if (converter->second->empty()) {
      // Remove converter when it's empty.
      master_converter_.RemoveInput(converter->second.get());
      converters_.erase(converter);
    }
  }
}

void AudioRendererMixer::AddErrorCallback(AudioRendererMixerInput* input) {
  base::AutoLock auto_lock(lock_);
  error_callbacks_.insert(input);
}

void AudioRendererMixer::RemoveErrorCallback(AudioRendererMixerInput* input) {
  base::AutoLock auto_lock(lock_);
  error_callbacks_.erase(input);
}

bool AudioRendererMixer::CurrentThreadIsRenderingThread() {
  return audio_sink_->CurrentThreadIsRenderingThread();
}

void AudioRendererMixer::SetPauseDelayForTesting(base::TimeDelta delay) {
  base::AutoLock auto_lock(lock_);
  pause_delay_ = delay;
}

int AudioRendererMixer::Render(base::TimeDelta delay,
                               base::TimeTicks delay_timestamp,
                               int prior_frames_skipped,
                               AudioBus* audio_bus) {
  TRACE_EVENT0("audio", "AudioRendererMixer::Render");
  base::AutoLock auto_lock(lock_);

  // If there are no mixer inputs and we haven't seen one for a while, pause the
  // sink to avoid wasting resources when media elements are present but remain
  // in the pause state.
  const base::TimeTicks now = base::TimeTicks::Now();
  if (!master_converter_.empty()) {
    last_play_time_ = now;
  } else if (now - last_play_time_ >= pause_delay_ && playing_) {
    audio_sink_->Pause();
    if (muted_suspender_)
      muted_suspender_->OnPaused();
    playing_ = false;
  }

  // Since AudioConverter uses uint32_t for delay calculations, we must drop
  // negative delay values (which are incorrect anyways).
  if (delay < base::TimeDelta())
    delay = base::TimeDelta();

  uint32_t frames_delayed =
      AudioTimestampHelper::TimeToFrames(delay, output_params_.sample_rate());
  master_converter_.ConvertWithDelay(frames_delayed, audio_bus);
  return audio_bus->frames();
}

void AudioRendererMixer::OnRenderError() {
  // Call each mixer input and signal an error.
  base::AutoLock auto_lock(lock_);
  for (auto* input : error_callbacks_)
    input->OnRenderError();
}

scoped_refptr<base::SingleThreadTaskRunner>
AudioRendererMixer::GetSuspenderTaskRunner() {
  if (!suspender_task_runner_) {
    suspender_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
        {base::TaskPriority::USER_VISIBLE,
         base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
  }
  return suspender_task_runner_;
}

}  // namespace media
