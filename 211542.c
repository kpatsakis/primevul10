  bool shouldSkipBuiltins() const override {
    return m_flags & NoTrackBuiltins;
  }