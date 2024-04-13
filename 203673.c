cmd_file (const char *com _GL_UNUSED, const char *val, void *place)
{
  char **pstring = (char **)place;

  xfree_null (*pstring);

  /* #### If VAL is empty, perhaps should set *PLACE to NULL.  */

  *pstring = xstrdup (val);

#if defined(WINDOWS) || defined(MSDOS)
  /* Convert "\" to "/". */
  {
    char *s;
    for (s = *pstring; *s; s++)
      if (*s == '\\')
        *s = '/';
  }
#endif
  return true;
}