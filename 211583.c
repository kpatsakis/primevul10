String HHVM_FUNCTION(convert_cyr_string,
                     const String& str,
                     const String& from,
                     const String& to) {
  return string_convert_cyrillic_string(str, from[0], to[0]);
}