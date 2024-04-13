static String copy_and_clear(UncheckedBuffer &buf) {
  auto ret = buf.size() > 0 ? buf.copy() : empty_string();
  buf.clear();
  return ret;
}