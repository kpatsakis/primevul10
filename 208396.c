  FUNC_ATTR_NONNULL_RET FUNC_ATTR_NONNULL_ARG(1) FUNC_ATTR_WARN_UNUSED_RESULT
{
  if (dir == NULL || dir_len == 0) {
    return dest;
  }
  dest = strcpy_comma_escaped(dest, dir, dir_len);
  if (append_nvim) {
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
  }
  *dest++ = ',';
  return dest;
}