glob_char_p (s)
     const char *s;
{
  switch (*s)
    {
    case '*':
    case '[':
    case ']':
    case '?':
    case '\\':
      return 1;
    case '+':
    case '@':
    case '!':
      if (s[1] == '(')	/*(*/
	return 1;
      break;
    }
  return 0;
}