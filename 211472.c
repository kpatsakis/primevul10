static int compare_left(char const **a, char const *aend,
                        char const **b, char const *bend) {
  /* Compare two left-aligned numbers: the first to have a
     different value wins. */
  for(;; (*a)++, (*b)++) {
    if ((*a == aend || !isdigit((int)(unsigned char)**a)) &&
        (*b == bend || !isdigit((int)(unsigned char)**b)))
      return 0;
    else if (*a == aend || !isdigit((int)(unsigned char)**a))
      return -1;
    else if (*b == bend || !isdigit((int)(unsigned char)**b))
      return +1;
    else if (**a < **b)
      return -1;
    else if (**a > **b)
      return +1;
  }

  return 0;
}