void IterateUnicodeString(const string& str, UConverter* converter,
                          std::function<void(UChar32, int, bool)> callback) {
  const char* source = str.data();
  const char* limit = str.data() + str.length();
  UErrorCode status = U_ZERO_ERROR;

  UConverterToUCallback oldAction = nullptr;
  const void* oldContext = nullptr;
  bool format_error = false;

  // Subtle. You can't make a function pointer from a std::function. :-(
  // Instead, we pass the boolean pointer as the "context" object.
  ucnv_setToUCallBack(converter, unicode_error_callback, &format_error,
                      &oldAction, &oldContext, &status);
  if (U_FAILURE(status)) {
    LOG(ERROR) << "Could not set unicode error callback on converter";
    return;
  }

  while (source < limit) {
    const char* source_pre_fetch = source;
    // Note: ucnv_getNextUChar returns 0xFFFD on an encoding error.
    UChar32 next_char = ucnv_getNextUChar(converter, &source, limit, &status);
    if (U_FAILURE(status)) {
      source = limit;
    }
    int bytes_consumed = source - source_pre_fetch;
    callback(next_char, bytes_consumed, format_error);
    format_error = false;
  }

  ucnv_setToUCallBack(converter, oldAction, oldContext, nullptr, nullptr,
                      &status);
}