simple_atof (const char *beg, const char *end, double *dest)
{
  double result = 0;

  bool negative = false;
  bool seen_dot = false;
  bool seen_digit = false;
  double divider = 1;

  const char *p = beg;

  while (p < end && c_isspace (*p))
    ++p;
  if (p < end && (*p == '-' || *p == '+'))
    {
      negative = (*p == '-');
      ++p;
    }

  for (; p < end; p++)
    {
      char ch = *p;
      if (c_isdigit (ch))
        {
          if (!seen_dot)
            result = (10 * result) + (ch - '0');
          else
            result += (ch - '0') / (divider *= 10);
          seen_digit = true;
        }
      else if (ch == '.')
        {
          if (!seen_dot)
            seen_dot = true;
          else
            return false;
        }
      else
        return false;
    }
  if (!seen_digit)
    return false;
  if (negative)
    result = -result;

  *dest = result;
  return true;
}