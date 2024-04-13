static int64_t read_integer(const Array& args, const String& key, int64_t def) {
  if (args.exists(key)) {
    return args[key].toInt64();
  }
  return def;
}