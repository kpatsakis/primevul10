  void requestInit() override {
    str.reset();
    pos = 0;
    memset(&mask, 0, sizeof(mask));
  }