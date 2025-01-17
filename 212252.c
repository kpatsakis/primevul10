static int exif_process_string(char **result, char *value,
                               size_t byte_count) {
  /* we cannot use strlcpy - here the problem is that we cannot use strlen to
   * determin length of string and we cannot use strlcpy with len=byte_count+1
   * because then we might get into an EXCEPTION if we exceed an allocated
   * memory page...so we use php_strnlen in conjunction with memcpy and add
   * the NUL char.
   */
    if ((byte_count=php_strnlen(value, byte_count)) > 0) {
      return exif_process_undefined(result, value, byte_count);
    }
    PHP_STRNDUP((*result), "", 1); /* force empty string */
    if (*result) return byte_count+1;
    return 0;
}