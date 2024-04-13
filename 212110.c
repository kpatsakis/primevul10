inline static void appendchar(char **buffer, int *pos, int *size, char add) {
  if ((*pos + 1) >= *size) {
    *size <<= 1;
    *buffer = (char*)realloc(*buffer, *size);
  }
  (*buffer)[(*pos)++] = add;
}