static int getCharFromString(void *data) {
  char *p;
  int c;

  p = *(char **)data;
  if (*p) {
    c = *p++;
    *(char **)data = p;
  } else {
    c = EOF;
  }
  return c;
}