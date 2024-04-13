  void MemoryInfo(MemoryTracker* tracker) const override {
    tracker->TrackField("continuation_data", continuation_data);
  }