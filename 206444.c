void TLSWrap::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("error", error_);
  tracker->TrackField("pending_cleartext_input", pending_cleartext_input_);
  if (enc_in_ != nullptr)
    tracker->TrackField("enc_in", crypto::NodeBIO::FromBIO(enc_in_));
  if (enc_out_ != nullptr)
    tracker->TrackField("enc_out", crypto::NodeBIO::FromBIO(enc_out_));
}