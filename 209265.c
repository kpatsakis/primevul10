CharCodeToUnicode::CharCodeToUnicode(GString *tagA, Unicode *mapA,
				     CharCode mapLenA, GBool copyMap,
				     CharCodeToUnicodeString *sMapA,
				     int sMapLenA, int sMapSizeA) {
  tag = tagA;
  mapLen = mapLenA;
  if (copyMap) {
    map = (Unicode *)gmallocn(mapLen, sizeof(Unicode));
    memcpy(map, mapA, mapLen * sizeof(Unicode));
  } else {
    map = mapA;
  }
  sMap = sMapA;
  sMapLen = sMapLenA;
  sMapSize = sMapSizeA;
  refCnt = 1;
#if MULTITHREADED
  gInitMutex(&mutex);
#endif
}