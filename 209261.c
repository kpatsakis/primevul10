static int getCharFromFile(void *data) {
  return fgetc((FILE *)data);
}