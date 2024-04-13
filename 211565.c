String HHVM_FUNCTION(html_entity_decode,
                     const String& str,
                     int flags /* = k_ENT_HTML_QUOTE_DOUBLE */,
                     const String& charset /* = "UTF-8" */) {
  const char *scharset = charset.data();
  if (!*scharset) scharset = "ISO-8859-1";
  return StringUtil::HtmlDecode(str, StringUtil::toQuoteStyle(flags),
                                scharset, true);
}