    void MemoryInfo(MemoryTracker* tracker) const override {
      tracker->TrackField("promise", promise_);
      tracker->TrackField("ref", ref_);
    }