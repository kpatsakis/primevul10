cmd_number (const char *com, const char *val, void *place)
{
  if (!simple_atoi (val, val + strlen (val), place)
      || *(int *) place < 0)
    {
      fprintf (stderr, _("%s: %s: Invalid number %s.\n"),
               exec_name, com, quote (val));
      return false;
    }
  return true;
}