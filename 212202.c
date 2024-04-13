char *string_bin2hex(const char *input, int &len) {
  if (!len) return nullptr;
  int inLen = len;
  int outLen = inLen * 2;
  char* result = (char*)malloc(outLen + 1);
  len = outLen;
  return string_bin2hex(input, inLen, result);
}