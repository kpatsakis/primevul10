String HHVM_FUNCTION(ucfirst,
                     const String& str) {
  return stringToCaseFirst(str, toupper, isupper);
}