  void init(const string& name) {
    if (converter_ && name == name_) {
      // Note: this reset is not typically needed, but if not done, then in some
      // cases the cached converter will maintain state of input endianness
      // which isn't valid from input to input in every batched case.
      ucnv_reset(converter_);
      return;
    }

    if (converter_) {
      ucnv_close(converter_);
      converter_ = nullptr;
      name_ = "";
    }

    UErrorCode status = U_ZERO_ERROR;
    converter_ = ucnv_open(name.c_str(), &status);
    if (U_FAILURE(status)) {
      if (converter_) {
        ucnv_close(converter_);
        converter_ = nullptr;
      }
    } else {
      name_ = name;
    }
  }