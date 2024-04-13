char *string_rot13(const char *input, int len) {
  assert(input);

  static char rot13_from[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static char rot13_to[] =
    "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

  if (len == 0) {
    return nullptr;
  }
  char *ret = string_duplicate(input, len);
  string_translate(ret, len, rot13_from, rot13_to, 52);
  return ret;
}