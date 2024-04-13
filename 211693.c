Variant HHVM_FUNCTION(convert_uuencode,
                      const String& data) {
  if (data.empty()) return false;
  return StringUtil::UUEncode(data);
}