  void MemoryInfo(MemoryTracker* tracker) const override {
    tracker->TrackField("current_read", current_read_);
  }