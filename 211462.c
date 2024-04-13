String encode_as_utf8(int code_point) {
  auto res = folly::codePointToUtf8(code_point);
  return String(res);
}