  void TranslateCodepoints(icu::UnicodeString* s, bool* found_any_format_error,
                           UChar32 ch, int src_bytes, bool format_error) {
    if (ShouldHandleFormatError(error_options_, ch, format_error)) {
      *found_any_format_error = true;
      if (error_options_.elide_replacement) {
        return;
      } else {
        ch = error_options_.subst;
      }
    }
    s->append(ch);
  }