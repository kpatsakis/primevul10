pget_line (size_t indent, int rfc934_nesting, bool strip_trailing_cr,
	   bool pass_comments_through)
{
  FILE *fp = pfp;
  int c;
  size_t i;
  char *b;
  size_t s;

  do
    {
      i = 0;
      for (;;)
	{
	  c = getc (fp);
	  if (c == EOF)
	    {
	      if (ferror (fp))
		read_fatal ();
	      return 0;
	    }
	  if (indent <= i)
	    break;
	  if (c == ' ' || c == 'X')
	    i++;
	  else if (c == '\t')
	    i = (i + 8) & ~7;
	  else
	    break;
	}

      i = 0;
      b = buf;

      while (c == '-' && 0 <= --rfc934_nesting)
	{
	  c = getc (fp);
	  if (c == EOF)
	    goto patch_ends_in_middle_of_line;
	  if (c != ' ')
	    {
	      i = 1;
	      b[0] = '-';
	      break;
	    }
	  c = getc (fp);
	  if (c == EOF)
	    goto patch_ends_in_middle_of_line;
	}

      s = bufsize;

      for (;;)
	{
	  if (i == s - 1)
	    {
	      s *= 2;
	      b = realloc (b, s);
	      if (!b)
		{
		  if (!using_plan_a)
		    xalloc_die ();
		  return (size_t) -1;
		}
	      buf = b;
	      bufsize = s;
	    }
	  b[i++] = c;
	  if (c == '\n')
	    break;
	  c = getc (fp);
	  if (c == EOF)
	    goto patch_ends_in_middle_of_line;
	}

      p_input_line++;
    }
  while (*b == '#' && !pass_comments_through);

  if (strip_trailing_cr && 2 <= i && b[i - 2] == '\r')
    b[i-- - 2] = '\n';
  b[i] = '\0';
  return i;

 patch_ends_in_middle_of_line:
  if (ferror (fp))
    read_fatal ();
  say ("patch unexpectedly ends in middle of line\n");
  return 0;
}