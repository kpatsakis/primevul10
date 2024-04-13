inline static void appendsimplestring(char **buffer, int *pos, int *size,
                                      const char *add, int len) {
  int req_size = *pos + len;

  if (req_size > *size) {
    while (req_size > *size) {
      *size <<= 1;
    }
    *buffer = (char *)realloc(*buffer, *size);
  }
  memcpy(&(*buffer)[*pos], add, len);
  *pos += len;
}