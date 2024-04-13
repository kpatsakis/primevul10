CharCodeToUnicode::CharCodeToUnicode(GString *tagA) {
  CharCode i;

  tag = tagA;
  mapLen = 256;
  map = (Unicode *)gmallocn(mapLen, sizeof(Unicode));
  for (i = 0; i < mapLen; ++i) {
    map[i] = 0;
  }
  sMap = NULL;
  sMapLen = sMapSize = 0;
  refCnt = 1;
#if MULTITHREADED
  gInitMutex(&mutex);
#endif
}