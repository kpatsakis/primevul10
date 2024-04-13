String HHVM_FUNCTION(chop,
                      const String& str,
                      const String& charlist /* = k_HPHP_TRIM_CHARLIST */) {
  return stringTrim<false,true>(str, charlist);
}