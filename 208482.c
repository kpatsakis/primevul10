  FUNC_ATTR_WARN_UNUSED_RESULT FUNC_ATTR_NONNULL_RET FUNC_ATTR_NONNULL_ARG(1)
{
  if (val == NULL || *val == NUL) {
    return dest;
  }
  const void *iter = NULL;
  do {
    size_t dir_len;
    const char *dir;
    iter = (forward ? vim_colon_env_iter : vim_colon_env_iter_rev)(
        val, iter, &dir, &dir_len);
    if (dir != NULL && dir_len > 0) {
      dest = strcpy_comma_escaped(dest, dir, dir_len);
      if (!after_pathsep(dest - 1, dest)) {
        *dest++ = PATHSEP;
      }
      memmove(dest, "nvim", NVIM_SIZE);
      dest += NVIM_SIZE;
      if (suf1 != NULL) {
        *dest++ = PATHSEP;
        memmove(dest, suf1, len1);
        dest += len1;
        if (suf2 != NULL) {
          *dest++ = PATHSEP;
          memmove(dest, suf2, len2);
          dest += len2;
        }
      }
      *dest++ = ',';
    }
  } while (iter != NULL);
  return dest;
}