next_brace_sub (const char *cp, int flags)
{
  size_t depth = 0;
  while (*cp != '\0')
    if ((flags & GLOB_NOESCAPE) == 0 && *cp == '\\')
      {
        if (*++cp == '\0')
          break;
        ++cp;
      }
    else
      {
        if ((*cp == '}' && depth-- == 0) || (*cp == ',' && depth == 0))
          break;

        if (*cp++ == '{')
          depth++;
      }

  return *cp != '\0' ? cp : NULL;
}