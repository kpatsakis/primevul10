char *string_bin2hex(const char *input, int len, char* result) {
  static char hexconvtab[] = "0123456789abcdef";

  assert(input);
  if (len == 0) {
    return nullptr;
  }

  int i, j;
  for (i = j = 0; i < len; i++) {
    result[j++] = hexconvtab[(unsigned char)input[i] >> 4];
    result[j++] = hexconvtab[(unsigned char)input[i] & 15];
  }
  result[j] = '\0';
  return result;
}