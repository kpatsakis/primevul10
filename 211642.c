char * ap_php_conv_10(register int64_t num, register bool is_unsigned,
                      register int * is_negative, char *buf_end,
                      register int *len) {
  register char *p = buf_end;
  register uint64_t magnitude;

  if (is_unsigned) {
    magnitude = (uint64_t) num;
    *is_negative = 0;
  } else {
    *is_negative = (num < 0);

    /*
     * On a 2's complement machine, negating the most negative integer
     * results in a number that cannot be represented as a signed integer.
     * Here is what we do to obtain the number's magnitude:
     *      a. add 1 to the number
     *      b. negate it (becomes positive)
     *      c. convert it to unsigned
     *      d. add 1
     */
    if (*is_negative) {
      int64_t t = num + 1;
      magnitude = ((uint64_t) - t) + 1;
    } else {
      magnitude = (uint64_t) num;
    }
  }

  /*
   * We use a do-while loop so that we write at least 1 digit
   */
  do {
    register uint64_t new_magnitude = magnitude / 10;

    *--p = (char)(magnitude - new_magnitude * 10 + '0');
    magnitude = new_magnitude;
  }
  while (magnitude);

  *len = buf_end - p;
  return (p);
}