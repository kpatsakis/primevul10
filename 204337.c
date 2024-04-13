from_header (char const *where0, size_t digs, char const *type,
	     intmax_t minval, uintmax_t maxval,
	     bool octal_only, bool silent)
{
  uintmax_t value;
  uintmax_t uminval = minval;
  uintmax_t minus_minval = - uminval;
  char const *where = where0;
  char const *lim = where + digs;
  bool negative = false;

  /* Accommodate buggy tar of unknown vintage, which outputs leading
     NUL if the previous field overflows.  */
  where += !*where;

  /* Accommodate older tars, which output leading spaces.  */
  for (;;)
    {
      if (where == lim)
	{
	  if (type && !silent)
	    ERROR ((0, 0,
		    /* TRANSLATORS: %s is type of the value (gid_t, uid_t,
		       etc.) */
		    _("Blanks in header where numeric %s value expected"),
		    type));
	  return -1;
	}
      if (!isspace ((unsigned char) *where))
	break;
      where++;
    }

  value = 0;
  if (ISODIGIT (*where))
    {
      char const *where1 = where;
      bool overflow = false;

      for (;;)
	{
	  value += *where++ - '0';
	  if (where == lim || ! ISODIGIT (*where))
	    break;
	  overflow |= value != (value << LG_8 >> LG_8);
	  value <<= LG_8;
	}

      /* Parse the output of older, unportable tars, which generate
         negative values in two's complement octal.  If the leading
         nonzero digit is 1, we can't recover the original value
         reliably; so do this only if the digit is 2 or more.  This
         catches the common case of 32-bit negative time stamps.  */
      if ((overflow || maxval < value) && '2' <= *where1 && type)
	{
	  /* Compute the negative of the input value, assuming two's
	     complement.  */
	  int digit = (*where1 - '0') | 4;
	  overflow = 0;
	  value = 0;
	  where = where1;
	  for (;;)
	    {
	      value += 7 - digit;
	      where++;
	      if (where == lim || ! ISODIGIT (*where))
		break;
	      digit = *where - '0';
	      overflow |= value != (value << LG_8 >> LG_8);
	      value <<= LG_8;
	    }
	  value++;
	  overflow |= !value;

	  if (!overflow && value <= minus_minval)
	    {
	      if (!silent)
		WARN ((0, 0,
		       /* TRANSLATORS: Second %s is a type name (gid_t,uid_t,etc.) */
		       _("Archive octal value %.*s is out of %s range; assuming two's complement"),
		       (int) (where - where1), where1, type));
	      negative = true;
	    }
	}

      if (overflow)
	{
	  if (type && !silent)
	    ERROR ((0, 0,
		    /* TRANSLATORS: Second %s is a type name (gid_t,uid_t,etc.) */
		    _("Archive octal value %.*s is out of %s range"),
		    (int) (where - where1), where1, type));
	  return -1;
	}
    }
  else if (octal_only)
    {
      /* Suppress the following extensions.  */
    }
  else if (*where == '-' || *where == '+')
    {
      /* Parse base-64 output produced only by tar test versions
	 1.13.6 (1999-08-11) through 1.13.11 (1999-08-23).
	 Support for this will be withdrawn in future releases.  */
      int dig;
      if (!silent)
	{
	  static bool warned_once;
	  if (! warned_once)
	    {
	      warned_once = true;
	      WARN ((0, 0, _("Archive contains obsolescent base-64 headers")));
	    }
	}
      negative = *where++ == '-';
      while (where != lim
	     && (dig = base64_map[(unsigned char) *where]) < 64)
	{
	  if (value << LG_64 >> LG_64 != value)
	    {
	      char *string = alloca (digs + 1);
	      memcpy (string, where0, digs);
	      string[digs] = '\0';
	      if (type && !silent)
		ERROR ((0, 0,
			_("Archive signed base-64 string %s is out of %s range"),
			quote (string), type));
	      return -1;
	    }
	  value = (value << LG_64) | dig;
	  where++;
	}
    }
  else if (*where == '\200' /* positive base-256 */
	   || *where == '\377' /* negative base-256 */)
    {
      /* Parse base-256 output.  A nonnegative number N is
	 represented as (256**DIGS)/2 + N; a negative number -N is
	 represented as (256**DIGS) - N, i.e. as two's complement.
	 The representation guarantees that the leading bit is
	 always on, so that we don't confuse this format with the
	 others (assuming ASCII bytes of 8 bits or more).  */
      int signbit = *where & (1 << (LG_256 - 2));
      uintmax_t topbits = (((uintmax_t) - signbit)
			   << (CHAR_BIT * sizeof (uintmax_t)
			       - LG_256 - (LG_256 - 2)));
      value = (*where++ & ((1 << (LG_256 - 2)) - 1)) - signbit;
      for (;;)
	{
	  value = (value << LG_256) + (unsigned char) *where++;
	  if (where == lim)
	    break;
	  if (((value << LG_256 >> LG_256) | topbits) != value)
	    {
	      if (type && !silent)
		ERROR ((0, 0,
			_("Archive base-256 value is out of %s range"),
			type));
	      return -1;
	    }
	}
      negative = signbit != 0;
      if (negative)
	value = -value;
    }

  if (where != lim && *where && !isspace ((unsigned char) *where))
    {
      if (type)
	{
	  char buf[1000]; /* Big enough to represent any header.  */
	  static struct quoting_options *o;

	  if (!o)
	    {
	      o = clone_quoting_options (0);
	      set_quoting_style (o, locale_quoting_style);
	    }

	  while (where0 != lim && ! lim[-1])
	    lim--;
	  quotearg_buffer (buf, sizeof buf, where0, lim - where0, o);
	  if (!silent)
	    ERROR ((0, 0,
		    /* TRANSLATORS: Second %s is a type name (gid_t,uid_t,etc.) */
		    _("Archive contains %.*s where numeric %s value expected"),
		    (int) sizeof buf, buf, type));
	}

      return -1;
    }

  if (value <= (negative ? minus_minval : maxval))
    return represent_uintmax (negative ? -value : value);

  if (type && !silent)
    {
      char minval_buf[UINTMAX_STRSIZE_BOUND + 1];
      char maxval_buf[UINTMAX_STRSIZE_BOUND];
      char value_buf[UINTMAX_STRSIZE_BOUND + 1];
      char *minval_string = STRINGIFY_BIGINT (minus_minval, minval_buf + 1);
      char *value_string = STRINGIFY_BIGINT (value, value_buf + 1);
      if (negative)
	*--value_string = '-';
      if (minus_minval)
	*--minval_string = '-';
      /* TRANSLATORS: Second %s is type name (gid_t,uid_t,etc.) */
      ERROR ((0, 0, _("Archive value %s is out of %s range %s..%s"),
	      value_string, type,
	      minval_string, STRINGIFY_BIGINT (maxval, maxval_buf)));
    }

  return -1;
}