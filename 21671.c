unquoted_glob_pattern_p (string)
     register char *string;
{
  register int c;
  char *send;
  int open, bsquote;

  DECLARE_MBSTATE;

  open = bsquote = 0;
  send = string + strlen (string);

  while (c = *string++)
    {
      switch (c)
	{
	case '?':
	case '*':
	  return (1);

	case '[':
	  open++;
	  continue;

	case ']':
	  if (open)		/* XXX - if --open == 0? */
	    return (1);
	  continue;

	case '/':
	  if (open)
	    open = 0;

	case '+':
	case '@':
	case '!':
	  if (*string == '(')	/*)*/
	    return (1);
	  continue;

	/* A pattern can't end with a backslash, but a backslash in the pattern
	   can be removed by the matching engine, so we have to run it through
	   globbing. */
	case '\\':
	  if (*string != '\0' && *string != '/')
	    {
	      bsquote = 1;
	      string++;
	      continue;
	    }
	  else if (open && *string == '/')
	    {
	      string++;		/* quoted slashes in bracket expressions are ok */
	      continue;
	    }
	  else if (*string == 0)
	    return (0);
	 	  
	case CTLESC:
	  if (*string++ == '\0')
	    return (0);
	}

      /* Advance one fewer byte than an entire multibyte character to
	 account for the auto-increment in the loop above. */
#ifdef HANDLE_MULTIBYTE
      string--;
      ADVANCE_CHAR_P (string, send - string);
      string++;
#else
      ADVANCE_CHAR_P (string, send - string);
#endif
    }

  return ((bsquote && posix_glob_backslash) ? 2 : 0);
}