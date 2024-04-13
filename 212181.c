uint16_t inline PatAndRepl::hash(int start, int len) const {
  assertx(pat.size() >= start + len);
  return strtr_hash(pat.data() + start, len);
};