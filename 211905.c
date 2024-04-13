String HHVM_FUNCTION(ltrim,
                     const String& str,
                     const String& charlist /* = k_HPHP_TRIM_CHARLIST */) {
  return stringTrim<true,false>(str, charlist);
}