  void ReadValue(int64 batch, int64 n, tstring* out) const override {
    CopyToString(values_(batch, n), out);
  }