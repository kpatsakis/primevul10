  Frame *allocateFrame() override {
    return new HierarchicalProfilerFrame();
  }