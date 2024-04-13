  void Transcode(tstring* s, UConverter* input_encoder,
                 bool* found_any_format_error) {
    icu::UnicodeString source;
    IterateUnicodeString(
        *s, input_encoder,
        std::bind(&UnicodeTranscodeOp::TranslateCodepoints, this, &source,
                  found_any_format_error, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));

    Encode(output_encoding_, source, s);
  }