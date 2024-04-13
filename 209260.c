CharCodeToUnicode *CharCodeToUnicode::parseCIDToUnicode(GString *fileName,
							GString *collection) {
  FILE *f;
  Unicode *mapA;
  CharCode size, mapLenA;
  char buf[64];
  Unicode u;
  CharCodeToUnicode *ctu;

  if (!(f = fopen(fileName->getCString(), "r"))) {
    error(-1, "Couldn't open cidToUnicode file '%s'",
	  fileName->getCString());
    return NULL;
  }

  size = 32768;
  mapA = (Unicode *)gmallocn(size, sizeof(Unicode));
  mapLenA = 0;

  while (getLine(buf, sizeof(buf), f)) {
    if (mapLenA == size) {
      size *= 2;
      mapA = (Unicode *)greallocn(mapA, size, sizeof(Unicode));
    }
    if (sscanf(buf, "%x", &u) == 1) {
      mapA[mapLenA] = u;
    } else {
      error(-1, "Bad line (%d) in cidToUnicode file '%s'",
	    (int)(mapLenA + 1), fileName->getCString());
      mapA[mapLenA] = 0;
    }
    ++mapLenA;
  }
  fclose(f);

  ctu = new CharCodeToUnicode(collection->copy(), mapA, mapLenA, gTrue,
			      NULL, 0, 0);
  gfree(mapA);
  return ctu;
}