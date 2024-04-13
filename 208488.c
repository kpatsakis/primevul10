static int opt_strings_flags(
    char_u *val,             /* new value */
    char **values,           /* array of valid string values */
    unsigned *flagp,
    bool list                /* when TRUE: accept a list of values */
)
{
  unsigned int new_flags = 0;

  while (*val) {
    for (unsigned int i = 0;; ++i) {
      if (values[i] == NULL)            /* val not found in values[] */
        return FAIL;

      size_t len = STRLEN(values[i]);
      if (STRNCMP(values[i], val, len) == 0
          && ((list && val[len] == ',') || val[len] == NUL)) {
        val += len + (val[len] == ',');
        assert(i < sizeof(1U) * 8);
        new_flags |= (1U << i);
        break;                  /* check next item in val list */
      }
    }
  }
  if (flagp != NULL)
    *flagp = new_flags;

  return OK;
}