String HHVM_FUNCTION(htmlspecialchars_decode,
                     const String& str,
                     int flags /* = k_ENT_HTML_QUOTE_DOUBLE */) {
  return StringUtil::HtmlDecode(str, StringUtil::toQuoteStyle(flags),
                                "UTF-8", false);
}