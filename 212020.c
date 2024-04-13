String HHVM_FUNCTION(str_pad,
                     const String& input,
                     int pad_length,
                     const String& pad_string /* = " " */,
                     int pad_type /* = k_STR_PAD_RIGHT */) {
  return StringUtil::Pad(input, pad_length, pad_string,
                         (StringUtil::PadType)pad_type);
}