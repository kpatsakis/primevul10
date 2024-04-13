inline bool StringData::isStrictlyInteger(int64_t& res) const {
  // Exploit the NUL terminator and unsigned comparison. This single comparison
  // checks whether the string is empty or if the first byte is greater than '9'
  // or less than '-'. Note that '-' == 45 and '0' == 48, which makes this
  // valid. (46 == '.' and 47 == '/', so if one of those is the first byte, this
  // check will be a false positive, but it will still be caught later.)
  if ((unsigned char)(data()[0] - '-') > ('9' - '-')) {
    return false;
  }
  if (m_hash < 0) return false;
  auto const s = slice();
  return is_strictly_integer(s.data(), s.size(), res);
}