  void MemoryInfo(MemoryTracker* tracker) const override {
    tracker->TrackField("stats_field_array", stats_field_array_);
    tracker->TrackField("continuation_data", continuation_data);
  }