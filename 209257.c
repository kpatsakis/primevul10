void CharCodeToUnicode::setMapping(CharCode c, Unicode *u, int len) {
  int i, j;

  if (len == 1) {
    map[c] = u[0];
  } else {
    for (i = 0; i < sMapLen; ++i) {
      if (sMap[i].c == c) {
	break;
      }
    }
    if (i == sMapLen) {
      if (sMapLen == sMapSize) {
	sMapSize += 8;
	sMap = (CharCodeToUnicodeString *)
	         greallocn(sMap, sMapSize, sizeof(CharCodeToUnicodeString));
      }
      ++sMapLen;
    }
    map[c] = 0;
    sMap[i].c = c;
    sMap[i].len = len;
    for (j = 0; j < len && j < maxUnicodeString; ++j) {
      sMap[i].u[j] = u[j];
    }
  }
}