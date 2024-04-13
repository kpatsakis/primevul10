  int64 FeatureCount(int64 batch) const override {
    return row_splits_(batch + 1) - row_splits_(batch);
  }