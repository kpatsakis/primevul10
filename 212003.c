char * ap_php_conv_p2(register uint64_t num, register int nbits,
                      char format, char *buf_end, register int *len)
{
  register int mask = (1 << nbits) - 1;
  register char *p = buf_end;
  static char low_digits[] = "0123456789abcdef";
  static char upper_digits[] = "0123456789ABCDEF";
  register char *digits = (format == 'X') ? upper_digits : low_digits;

  do {
    *--p = digits[num & mask];
    num >>= nbits;
  }
  while (num);

  *len = buf_end - p;
  return (p);
}