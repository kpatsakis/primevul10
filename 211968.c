static int exif_process_undefined(char **result, char *value,
                                  size_t byte_count) {
  /* we cannot use strlcpy - here the problem is that we have to copy NUL
   * chars up to byte_count, we also have to add a single NUL character to
   * force end of string.
   */
  if (byte_count) {
    PHP_STRNDUP((*result), value, byte_count); /* NULL @ byte_count!!! */
    if (*result) return byte_count+1;
  }
  return 0;
}