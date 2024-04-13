String HHVM_FUNCTION(fb_htmlspecialchars,
                     const String& str,
                     int flags /* = k_ENT_HTML_QUOTE_DOUBLE */,
                     const String& charset /* = "ISO-8859-1" */,
                     const Variant& extra /* = varray[] */) {
  if (!extra.isNull() && !extra.isArray()) {
    raise_expected_array_warning("fb_htmlspecialchars");
  }
  const Array& arr_extra = extra.isNull() ? empty_varray() : extra.toArray();
  return StringUtil::HtmlEncodeExtra(str, StringUtil::toQuoteStyle(flags),
                                     charset.data(), false, arr_extra);
}