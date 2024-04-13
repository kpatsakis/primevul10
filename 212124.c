  void writeStats(Array &ret) override {
    for (auto const& sample : m_samples) {
      auto const& time = sample.first;
      char timestr[512];
      snprintf(timestr, sizeof(timestr), "%" PRId64 ".%06" PRId64,
               time.first, time.second);

      ret.set(String(timestr), String(sample.second));
    }
  }