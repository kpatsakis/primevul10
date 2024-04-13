String HHVM_FUNCTION(strtoupper,
                     const String& str) {
  return stringForEachFast(str, toupper);
}