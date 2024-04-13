simple_atoi (const char *beg, const char *end, int *dest)
{
  int result = 0;
  bool negative = false;
  const char *p = beg;

  while (p < end && c_isspace (*p))
    ++p;
  if (p < end && (*p == '-' || *p == '+'))
    {
      negative = (*p == '-');
      ++p;
    }
  if (p == end)
    return false;

  /* Read negative numbers in a separate loop because the most
     negative integer cannot be represented as a positive number.  */

  if (!negative)
    for (; p < end && c_isdigit (*p); p++)
      {
        int next = (10 * result) + (*p - '0');
        if (next < result)
          return false;         /* overflow */
        result = next;
      }
  else
    for (; p < end && c_isdigit (*p); p++)
      {
        int next = (10 * result) - (*p - '0');
        if (next > result)
          return false;         /* underflow */
        result = next;
      }

  if (p != end)
    return false;

  *dest = result;
  return true;
}