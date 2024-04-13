void unicode_error_callback(const void* context, UConverterToUnicodeArgs* args,
                            const char* codeUnits, int32_t length,
                            UConverterCallbackReason reason,
                            UErrorCode* pErrorCode) {
  // Careful: this depends on setting up the context settings when the
  // callback is registered.
  bool* format_error = const_cast<bool*>(static_cast<const bool*>(context));

  if (reason == UCNV_UNASSIGNED || reason == UCNV_ILLEGAL ||
      reason == UCNV_IRREGULAR) {
    *format_error = true;
  }

  // Side note: the default behavior in this case is that without a substitution
  // made by the callback, the UConverter will signal an error to the iterator
  // making the string iteration bail out. Instead, forward to the built-in
  // substitution handler.
  UCNV_TO_U_CALLBACK_SUBSTITUTE(nullptr, args, codeUnits, length, reason,
                                pErrorCode);
}