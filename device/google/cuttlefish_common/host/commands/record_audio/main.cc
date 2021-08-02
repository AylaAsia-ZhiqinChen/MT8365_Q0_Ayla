/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/vsoc/lib/audio_data_region_view.h"
#include "common/vsoc/lib/circqueue_impl.h"
#include "common/vsoc/lib/vsoc_audio_message.h"
#include "host/libs/config/cuttlefish_config.h"

#include "WaveWriter.h"

#include <android-base/logging.h>
#include <gflags/gflags.h>
#include <iostream>
#include <signal.h>

using AudioDataRegionView = vsoc::audio_data::AudioDataRegionView;
using WaveWriter = android::WaveWriter;

DEFINE_string(output_file, "", "Location of the output audio file.");
DEFINE_bool(verbose, false, "Enable verbose logging.");

volatile bool gDone = false;
static void SigIntHandler(int /* sig */) {
  gDone = true;
}

int main(int argc, char **argv) {
  ::android::base::InitLogging(argv, android::base::StderrLogger);
  google::ParseCommandLineFlags(&argc, &argv, true);

  LOG_IF(FATAL, FLAGS_output_file.empty())
      << "--output_file must be specified.";

  AudioDataRegionView *audio_data_rv =
      AudioDataRegionView::GetInstance(vsoc::GetDomain().c_str());

  auto worker = audio_data_rv->StartWorker();

  std::unique_ptr<WaveWriter> writer;
  int64_t frameCount = 0LL;

  // The configuration the writer is setup for.
  gce_audio_message writer_hdr;

  uint8_t buffer[4096];

  gDone = false;

  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = SigIntHandler;

  struct sigaction oact;
  sigaction(SIGINT, &act, &oact);

  while (!gDone) {
    intptr_t res = audio_data_rv->data()->audio_queue.Read(
            audio_data_rv,
            reinterpret_cast<char *>(buffer),
            sizeof(buffer));

    if (res < 0) {
      std::cerr << "CircularPacketQueue::Read returned " << res << std::endl;
        continue;
    }

    CHECK_GE(static_cast<size_t>(res), sizeof(gce_audio_message));

    gce_audio_message hdr;
    std::memcpy(&hdr, buffer, sizeof(gce_audio_message));

    if (hdr.message_type != gce_audio_message::DATA_SAMPLES) {
        continue;
    }

    const size_t payloadSize = res - sizeof(gce_audio_message);

    if (FLAGS_verbose) {
      std::cerr
          << "stream "
          << hdr.stream_number
          << ", frame "
          << hdr.frame_num
          << ", rate "
          << hdr.frame_rate
          << ", channel_mask "
          << hdr.channel_mask
          << ", format "
          << hdr.format
          << ", payload_size "
          << payloadSize
          << std::endl;
    }

    if (!writer) {
      const size_t numChannels = hdr.frame_size / sizeof(int16_t);

      writer.reset(
          new WaveWriter(FLAGS_output_file.c_str(), numChannels, hdr.frame_rate));

      frameCount = hdr.frame_num;
      writer_hdr = hdr;
    } else if (writer_hdr.frame_size != hdr.frame_size
        || writer_hdr.frame_rate != hdr.frame_rate
        || writer_hdr.stream_number != hdr.stream_number) {
      std::cerr << "Audio configuration changed. Aborting." << std::endl;
      break;
    }

    int64_t framesMissing = hdr.frame_num - frameCount;
    if (framesMissing > 0) {
      // TODO(andih): Insert silence here, if necessary.
    }

    frameCount = hdr.frame_num;

    writer->Append(&buffer[sizeof(gce_audio_message)], payloadSize);
  }

  std::cout << "DONE" << std::endl;

  return 0;
}

