Variant HHVM_FUNCTION(money_format,
                      const String& format,
                      double number) {
  String s = StringUtil::MoneyFormat(format.c_str(), number);
  if (s.isNull()) return false;
  return s;
}