cmd_string_uppercase (const char *com _GL_UNUSED, const char *val, void *place)
{
  char *q, **pstring;
  pstring = (char **)place;
  xfree_null (*pstring);

  *pstring = xmalloc (strlen (val) + 1);

  for (q = *pstring; *val; val++, q++)
    *q = c_toupper (*val);

  *q = '\0';
  return true;
}