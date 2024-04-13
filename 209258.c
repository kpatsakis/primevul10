int CharCodeToUnicode::mapToUnicode(CharCode c, Unicode *u, int size) {
  int i, j;

  if (c >= mapLen) {
    return 0;
  }
  if (map[c]) {
    u[0] = map[c];
    return 1;
  }
  for (i = 0; i < sMapLen; ++i) {
    if (sMap[i].c == c) {
      for (j = 0; j < sMap[i].len && j < size; ++j) {
	u[j] = sMap[i].u[j];
      }
      return j;
    }
  }
  return 0;
}