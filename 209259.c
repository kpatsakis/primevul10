CharCodeToUnicode *CharCodeToUnicode::make8BitToUnicode(Unicode *toUnicode) {
  return new CharCodeToUnicode(NULL, toUnicode, 256, gTrue, NULL, 0, 0);
}