Variant HHVM_FUNCTION(convert_uudecode,
                      const String& data) {
  String ret = StringUtil::UUDecode(data);
  if (ret.isNull()) {
    raise_warning(
      "convert_uudecode(): "
      "The given parameter is not a valid uuencoded string");
    return false; // bad format
  }
  return ret;
}