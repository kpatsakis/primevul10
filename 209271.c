void CharCodeToUnicode::mergeCMap(GString *buf, int nBits) {
  char *p;

  p = buf->getCString();
  parseCMap1(&getCharFromString, &p, nBits);
}