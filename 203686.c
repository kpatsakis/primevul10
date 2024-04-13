cmd_time (const char *com, const char *val, void *place)
{
  double number, mult;
  const char *end = val + strlen (val);

  /* Strip trailing whitespace.  */
  while (val < end && c_isspace (end[-1]))
    --end;

  if (val == end)
    {
    err:
      fprintf (stderr, _("%s: %s: Invalid time period %s\n"),
               exec_name, com, quote (val));
      return false;
    }

  switch (c_tolower (end[-1]))
    {
    case 's':
      --end, mult = 1;          /* seconds */
      break;
    case 'm':
      --end, mult = 60;         /* minutes */
      break;
    case 'h':
      --end, mult = 3600;       /* hours */
      break;
    case 'd':
      --end, mult = 86400.0;    /* days */
      break;
    case 'w':
      --end, mult = 604800.0;   /* weeks */
      break;
    default:
      /* Not a recognized suffix: assume it belongs to the number.
         (If not, simple_atof will raise an error.)  */
      mult = 1;
    }

  /* Skip leading and trailing whitespace. */
  while (val < end && c_isspace (*val))
    ++val;
  while (val < end && c_isspace (end[-1]))
    --end;
  if (val == end)
    goto err;

  if (!simple_atof (val, end, &number))
    goto err;

  *(double *)place = number * mult;
  return true;
}