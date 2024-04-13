GBool CharCodeToUnicode::match(GString *tagA) {
  return tag && !tag->cmp(tagA);
}