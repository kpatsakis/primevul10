cmd_boolean (const char *com, const char *val, void *place)
{
  bool value;

  if (CMP2 (val, 'o', 'n') || CMP3 (val, 'y', 'e', 's') || CMP1 (val, '1'))
    /* "on", "yes" and "1" mean true. */
    value = true;
  else if (CMP3 (val, 'o', 'f', 'f') || CMP2 (val, 'n', 'o') || CMP1 (val, '0'))
    /* "off", "no" and "0" mean false. */
    value = false;
  else
    {
      fprintf (stderr,
               _("%s: %s: Invalid boolean %s; use `on' or `off'.\n"),
               exec_name, com, quote (val));
      return false;
    }

  *(bool *) place = value;
  return true;
}