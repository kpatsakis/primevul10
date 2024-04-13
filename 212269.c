static int compare_right(char const **a, char const *aend,
                         char const **b, char const *bend) {
  int bias = 0;

  /* The longest run of digits wins.  That aside, the greatest
     value wins, but we can't know that it will until we've scanned
     both numbers to know that they have the same magnitude, so we
     remember it in BIAS. */
  for(;; (*a)++, (*b)++) {
    if ((*a == aend || !isdigit((int)(unsigned char)**a)) &&
        (*b == bend || !isdigit((int)(unsigned char)**b)))
      return bias;
    else if (*a == aend || !isdigit((int)(unsigned char)**a))
      return -1;
    else if (*b == bend || !isdigit((int)(unsigned char)**b))
      return +1;
    else if (**a < **b) {
      if (!bias)
        bias = -1;
    } else if (**a > **b) {
      if (!bias)
        bias = +1;
    }
  }

  return 0;
}