CharCodeToUnicode *CharCodeToUnicode::parseUnicodeToUnicode(
						    GString *fileName) {
  FILE *f;
  Unicode *mapA;
  CharCodeToUnicodeString *sMapA;
  CharCode size, oldSize, len, sMapSizeA, sMapLenA;
  char buf[256];
  char *tok;
  Unicode u0;
  Unicode uBuf[maxUnicodeString];
  CharCodeToUnicode *ctu;
  int line, n, i;

  if (!(f = fopen(fileName->getCString(), "r"))) {
    error(-1, "Couldn't open unicodeToUnicode file '%s'",
	  fileName->getCString());
    return NULL;
  }

  size = 4096;
  mapA = (Unicode *)gmallocn(size, sizeof(Unicode));
  memset(mapA, 0, size * sizeof(Unicode));
  len = 0;
  sMapA = NULL;
  sMapSizeA = sMapLenA = 0;

  line = 0;
  while (getLine(buf, sizeof(buf), f)) {
    ++line;
    if (!(tok = strtok(buf, " \t\r\n")) ||
	sscanf(tok, "%x", &u0) != 1) {
      error(-1, "Bad line (%d) in unicodeToUnicode file '%s'",
	    line, fileName->getCString());
      continue;
    }
    n = 0;
    while (n < maxUnicodeString) {
      if (!(tok = strtok(NULL, " \t\r\n"))) {
	break;
      }
      if (sscanf(tok, "%x", &uBuf[n]) != 1) {
	error(-1, "Bad line (%d) in unicodeToUnicode file '%s'",
	      line, fileName->getCString());
	break;
      }
      ++n;
    }
    if (n < 1) {
      error(-1, "Bad line (%d) in unicodeToUnicode file '%s'",
	    line, fileName->getCString());
      continue;
    }
    if (u0 >= size) {
      oldSize = size;
      while (u0 >= size) {
	size *= 2;
      }
      mapA = (Unicode *)greallocn(mapA, size, sizeof(Unicode));
      memset(mapA + oldSize, 0, (size - oldSize) * sizeof(Unicode));
    }
    if (n == 1) {
      mapA[u0] = uBuf[0];
    } else {
      mapA[u0] = 0;
      if (sMapLenA == sMapSizeA) {
	sMapSizeA += 16;
	sMapA = (CharCodeToUnicodeString *)
	          greallocn(sMapA, sMapSizeA, sizeof(CharCodeToUnicodeString));
      }
      sMapA[sMapLenA].c = u0;
      for (i = 0; i < n; ++i) {
	sMapA[sMapLenA].u[i] = uBuf[i];
      }
      sMapA[sMapLenA].len = n;
      ++sMapLenA;
    }
    if (u0 >= len) {
      len = u0 + 1;
    }
  }
  fclose(f);

  ctu = new CharCodeToUnicode(fileName->copy(), mapA, len, gTrue,
			      sMapA, sMapLenA, sMapSizeA);
  gfree(mapA);
  return ctu;
}