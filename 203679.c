cmd_spec_useragent (const char *com, const char *val, void *place_ignored _GL_UNUSED)
{
  /* Disallow embedded newlines.  */
  if (strchr (val, '\n'))
    {
      fprintf (stderr, _("%s: %s: Invalid value %s.\n"),
               exec_name, com, quote (val));
      return false;
    }
  xfree_null (opt.useragent);
  opt.useragent = xstrdup (val);
  return true;
}