lt_argz_insertdir (char **pargz, size_t *pargz_len, const char *dirnam,
		   struct dirent *dp)
{
  char   *buf	    = 0;
  size_t buf_len    = 0;
  char   *end	    = 0;
  size_t end_offset = 0;
  size_t dir_len    = 0;
  int    errors	    = 0;

  assert (pargz);
  assert (pargz_len);
  assert (dp);

  dir_len = LT_STRLEN (dirnam);
  end     = dp->d_name + D_NAMLEN(dp);

  /* Ignore version numbers.  */
  {
    char *p;
    for (p = end; p -1 > dp->d_name; --p)
      if (strchr (".0123456789", p[-1]) == 0)
	break;

    if (*p == '.')
      end = p;
  }

  /* Ignore filename extension.  */
  {
    char *p;
    for (p = end -1; p > dp->d_name; --p)
      if (*p == '.')
	{
	  end = p;
	  break;
	}
  }

  /* Prepend the directory name.  */
  end_offset	= end - dp->d_name;
  buf_len	= dir_len + 1+ end_offset;
  buf		= MALLOC (char, 1+ buf_len);
  if (!buf)
    return ++errors;

  assert (buf);

  strcpy  (buf, dirnam);
  strcat  (buf, "/");
  strncat (buf, dp->d_name, end_offset);
  buf[buf_len] = LT_EOS_CHAR;

  /* Try to insert (in order) into ARGZ/ARGZ_LEN.  */
  if (lt_argz_insertinorder (pargz, pargz_len, buf) != 0)
    ++errors;

  FREE (buf);

  return errors;
}