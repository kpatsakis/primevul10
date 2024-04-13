int string_natural_cmp(char const *a, size_t a_len,
                       char const *b, size_t b_len, int fold_case) {
  char ca, cb;
  char const *ap, *bp;
  char const *aend = a + a_len, *bend = b + b_len;
  int fractional, result;

  if (a_len == 0 || b_len == 0)
    return a_len - b_len;

  ap = a;
  bp = b;
  while (1) {
    ca = *ap; cb = *bp;

    /* skip over leading spaces or zeros */
    while (isspace((int)(unsigned char)ca))
      ca = *++ap;

    while (isspace((int)(unsigned char)cb))
      cb = *++bp;

    /* process run of digits */
    if (isdigit((int)(unsigned char)ca)  &&  isdigit((int)(unsigned char)cb)) {
      fractional = (ca == '0' || cb == '0');

      if (fractional)
        result = compare_left(&ap, aend, &bp, bend);
      else
        result = compare_right(&ap, aend, &bp, bend);

      if (result != 0)
        return result;
      else if (ap == aend && bp == bend)
        /* End of the strings. Let caller sort them out. */
        return 0;
      else {
        /* Keep on comparing from the current point. */
        ca = *ap; cb = *bp;
      }
    }

    if (fold_case) {
      ca = toupper((int)(unsigned char)ca);
      cb = toupper((int)(unsigned char)cb);
    }

    if (ca < cb)
      return -1;
    else if (ca > cb)
      return +1;

    ++ap; ++bp;
    if (ap >= aend && bp >= bend)
      /* The strings compare the same.  Perhaps the caller
         will want to call strcmp to break the tie. */
      return 0;
    else if (ap >= aend)
      return -1;
    else if (bp >= bend)
      return 1;
  }
}