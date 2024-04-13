AudioManagerBase::~AudioManagerBase() {
  CHECK(!audio_thread_.get());
  DCHECK_EQ(0, num_output_streams_);
  DCHECK_EQ(0, num_input_streams_);
}
