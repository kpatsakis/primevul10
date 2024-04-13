String HHVM_FUNCTION(rtrim,
                     const String& str,
                     const String& charlist /* = k_HPHP_TRIM_CHARLIST */) {
  return stringTrim<false,true>(str, charlist);
}