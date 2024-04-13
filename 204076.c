line_terminator (const char *start _GL_UNUSED, const char *peeked, int peeklen)
{
  const char *p = memchr (peeked, '\n', peeklen);
  if (p)
    /* p+1 because the line must include '\n' */
    return p + 1;
  return NULL;
}