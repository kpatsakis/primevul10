Variant HHVM_FUNCTION(sscanf, const String& str, const String& format) {
  Variant ret;
  int result;
  result = string_sscanf(str.c_str(), format.c_str(), 0, ret);
  if (SCAN_ERROR_WRONG_PARAM_COUNT == result) return init_null();
  return ret;
}