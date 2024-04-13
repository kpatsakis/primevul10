cmd_string (const char *com _GL_UNUSED, const char *val, void *place)
{
  char **pstring = (char **)place;

  xfree_null (*pstring);
  *pstring = xstrdup (val);
  return true;
}