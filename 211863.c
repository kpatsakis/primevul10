String HHVM_FUNCTION(trim,
                     const String& str,
                     const String& charlist /* = k_HPHP_TRIM_CHARLIST */) {
  return stringTrim<true,true>(str, charlist);
}