void string_translate(char *str, int len, const char *str_from,
                      const char *str_to, int trlen) {
  int i;
  unsigned char xlat[256];

  if ((trlen < 1) || (len < 1)) {
    return;
  }

  for (i = 0; i < 256; xlat[i] = i, i++);
  for (i = 0; i < trlen; i++) {
    xlat[(unsigned char) str_from[i]] = str_to[i];
  }

  for (i = 0; i < len; i++) {
    str[i] = xlat[(unsigned char) str[i]];
  }
}