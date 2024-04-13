static Variant to_double(UncheckedBuffer &buf) {
  auto data = buf.data();
  auto ret = data ? zend_strtod(data, nullptr) : 0.0;
  buf.clear();
  return ret;
}