int string_ncmp(const char *s1, const char *s2, int len) {
  for (int i = 0; i < len; i++) {
    char c1 = s1[i];
    char c2 = s2[i];
    if (c1 > c2) return 1;
    if (c1 < c2) return -1;
  }
  return 0;
}