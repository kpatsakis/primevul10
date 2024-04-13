  void endFrame(const TypedValue* /*retval*/, const char* symbol,
                bool /*endMain*/ = false) override {
    doTrace(symbol, true);
  }