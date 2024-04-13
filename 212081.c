Array HHVM_FUNCTION(str_getcsv,
                    const String& str,
                    const String& delimiter /* = "," */,
                    const String& enclosure /* = "\"" */,
                    const String& escape /* = "\\" */) {
  if (str.empty()) {
    return make_varray(uninit_variant);
  }

  auto check_arg = [](const String& arg, char default_arg) {
    return arg.size() > 0 ? arg[0] : default_arg;
  };

  char delimiter_char = check_arg(delimiter, ',');
  char enclosure_char = check_arg(enclosure, '"');
  char escape_char = check_arg(escape, '\\');

  auto dummy = req::make<PlainFile>();
  return dummy->readCSV(0, delimiter_char, enclosure_char, escape_char, &str);
}