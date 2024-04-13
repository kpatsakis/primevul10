ere_char (c)
     int c;
{
  switch (c)
    {
    case '.':
    case '[':
    case '\\':
    case '(':
    case ')':
    case '*':
    case '+':
    case '?':
    case '{':
    case '|':
    case '^':
    case '$':
      return 1;
    default: 
      return 0;
    }
  return (0);
}