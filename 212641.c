  void ReadValue(int64 batch, int64 n, uint64* out) const override {
    CopyToFingerprint(values_(row_splits_(batch) + n), out);
  }