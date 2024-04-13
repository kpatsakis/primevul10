void AudioManagerBase::ReleaseInputStream(AudioInputStream* stream) {
  DCHECK(stream);
  num_input_streams_--;
  delete stream;
}
