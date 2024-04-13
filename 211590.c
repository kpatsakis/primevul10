String HHVM_FUNCTION(htmlspecialchars,
                     const String& str,
                     int flags /* = k_ENT_HTML_QUOTE_DOUBLE */,
                     const String& charset /* = "UTF-8" */,
                     bool double_encode /* = true */) {
  // dropping double_encode parameters and see runtime/base/zend-html.h
  const char *scharset = charset.data();
  if (!*scharset) scharset = "ISO-8859-1";
  return StringUtil::HtmlEncode(str, StringUtil::toQuoteStyleBitmask(flags),
                                scharset, double_encode, false);
}