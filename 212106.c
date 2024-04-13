String HHVM_FUNCTION(quoted_printable_encode,
                     const String& str) {
  return StringUtil::QuotedPrintableEncode(str);
}