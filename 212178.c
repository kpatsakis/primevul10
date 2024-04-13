String HHVM_FUNCTION(quoted_printable_decode,
                     const String& str) {
  return StringUtil::QuotedPrintableDecode(str);
}