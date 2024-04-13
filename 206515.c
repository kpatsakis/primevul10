  void MemoryInfo(MemoryTracker* tracker) const override {
    tracker->TrackField("paths", paths);
  }