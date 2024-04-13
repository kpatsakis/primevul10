quote_system_arg (char *quoted, char const *arg)
{
  char *q = quotearg_style (shell_quoting_style, arg);
  size_t len = strlen (q);

  if (quoted)
    memcpy (quoted, q, len + 1);
  return len;
}