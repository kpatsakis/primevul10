main_strtoxoff (const char* s, xoff_t *xo, char which)
{
  char *e;
  xoff_t x;

  XD3_ASSERT(s && *s != 0);

  {
    /* Should check LONG_MIN, LONG_MAX, LLONG_MIN, LLONG_MAX? */
#if SIZEOF_XOFF_T == 4
    long xx = strtol (s, &e, 0);
#else
    long long xx = strtoll (s, &e, 0);
#endif

    if (xx < 0)
      {
	XPR(NT "-%c: negative integer: %s\n", which, s);
	return EXIT_FAILURE;
      }

    x = xx;
  }

  if (*e != 0)
    {
      XPR(NT "-%c: invalid integer: %s\n", which, s);
      return EXIT_FAILURE;
    }

  (*xo) = x;
  return 0;
}