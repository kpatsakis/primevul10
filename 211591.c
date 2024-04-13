  static const StringData *GetIntegerStringData(int64_t n) {
    if (HasConverted(n)) {
      return *(converted_integers + n);
    }
    return nullptr;
  }