cmd_number_inf (const char *com, const char *val, void *place)
{
  if (!strcasecmp (val, "inf"))
    {
      *(int *) place = 0;
      return true;
    }
  return cmd_number (com, val, place);
}