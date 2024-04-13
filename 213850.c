  ~WrappedConverter() {
    if (converter_) {
      ucnv_close(converter_);
    }
  }