AudioInputStream* AudioManagerBase::MakeAudioInputStream(
    const AudioParameters& params, const std::string& device_id) {
  if (!params.IsValid() || (params.channels() > kMaxInputChannels) ||
      device_id.empty()) {
    DLOG(ERROR) << "Audio parameters are invalid for device " << device_id;
    return NULL;
  }

  if (num_input_streams_ >= max_num_input_streams_) {
    DLOG(ERROR) << "Number of opened input audio streams "
                << num_input_streams_
                << " exceed the max allowed number " << max_num_input_streams_;
    return NULL;
  }

  AudioInputStream* stream = NULL;
  if (params.format() == AudioParameters::AUDIO_MOCK) {
    stream = FakeAudioInputStream::MakeFakeStream(this, params);
  } else if (params.format() == AudioParameters::AUDIO_PCM_LINEAR) {
    stream = MakeLinearInputStream(params, device_id);
  } else if (params.format() == AudioParameters::AUDIO_PCM_LOW_LATENCY) {
    stream = MakeLowLatencyInputStream(params, device_id);
  }

  if (stream)
    ++num_input_streams_;

  return stream;
}
