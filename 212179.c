static const char *read_string(const Array& args, const String& key, const char *def,
                               std::vector<String> &strings) {
  if (args.exists(key)) {
    String value = args[key].toString();
    strings.push_back(value);
    return (char*)value.data();
  }
  return def;
}