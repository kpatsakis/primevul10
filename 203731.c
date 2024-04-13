get_ed_command_letter (char const *line)
{
  char const *p = line;
  char letter;
  bool pair = false;

  if (ISDIGIT (*p))
    {
      while (ISDIGIT (*++p))
	/* do nothing */ ;
      if (*p == ',')
	{
	  if (! ISDIGIT (*++p))
	    return 0;
	  while (ISDIGIT (*++p))
	    /* do nothing */ ;
	  pair = true;
	}
    }

  letter = *p++;

  switch (letter)
    {
    case 'a':
    case 'i':
      if (pair)
	return 0;
      break;

    case 'c':
    case 'd':
      break;

    case 's':
      if (strncmp (p, "/.//", 4) != 0)
	return 0;
      p += 4;
      break;

    default:
      return 0;
    }

  while (*p == ' ' || *p == '\t')
    p++;
  if (*p == '\n')
    return letter;
  return 0;
}