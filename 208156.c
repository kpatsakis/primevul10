main_atoux (const char* arg, xoff_t *xo, xoff_t low,
	    xoff_t high, char which)
{
  xoff_t x;
  int ret;

  if ((ret = main_strtoxoff (arg, & x, which))) { return ret; }

  if (x < low)
    {
      XPR(NT "-%c: minimum value: %"Q"u\n", which, low);
      return EXIT_FAILURE;
    }
  if (high != 0 && x > high)
    {
      XPR(NT "-%c: maximum value: %"Q"u\n", which, high);
      return EXIT_FAILURE;
    }
  (*xo) = x;
  return 0;
}