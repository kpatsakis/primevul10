check_user_specified_header (const char *s)
{
  const char *p;

  for (p = s; *p && *p != ':' && !c_isspace (*p); p++)
    ;
  /* The header MUST contain `:' preceded by at least one
     non-whitespace character.  */
  if (*p != ':' || p == s)
    return false;
  /* The header MUST NOT contain newlines.  */
  if (strchr (s, '\n'))
    return false;
  return true;
}