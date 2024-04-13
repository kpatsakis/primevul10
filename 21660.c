quote_string_for_globbing (pathname, qflags)
     const char *pathname;
     int qflags;
{
  char *temp;
  register int i, j;
  int cclass, collsym, equiv, c, last_was_backslash;
  int savei, savej;

  temp = (char *)xmalloc (2 * strlen (pathname) + 1);

  if ((qflags & QGLOB_CVTNULL) && QUOTED_NULL (pathname))
    {
      temp[0] = '\0';
      return temp;
    }

  cclass = collsym = equiv = last_was_backslash = 0;
  for (i = j = 0; pathname[i]; i++)
    {
      /* Fix for CTLESC at the end of the string? */
      if (pathname[i] == CTLESC && pathname[i+1] == '\0')
	{
	  temp[j++] = pathname[i++];
	  break;
	}
      /* If we are parsing regexp, turn CTLESC CTLESC into CTLESC. It's not an
	 ERE special character, so we should just be able to pass it through. */
      else if ((qflags & (QGLOB_REGEXP|QGLOB_CTLESC)) && pathname[i] == CTLESC && (pathname[i+1] == CTLESC || pathname[i+1] == CTLNUL))
	{
	  i++;
	  temp[j++] = pathname[i];
	  continue;
	}
      else if (pathname[i] == CTLESC)
	{
	  if ((qflags & QGLOB_FILENAME) && pathname[i+1] == '/')
	    continue;
	  /* What to do if preceding char is backslash? */
	  if (pathname[i+1] != CTLESC && (qflags & QGLOB_REGEXP) && ere_char (pathname[i+1]) == 0)
	    continue;
	  temp[j++] = '\\';
	  i++;
	  if (pathname[i] == '\0')
	    break;
	}
      else if ((qflags & QGLOB_REGEXP) && (i == 0 || pathname[i-1] != CTLESC) && pathname[i] == '[')	/*]*/
	{
	  temp[j++] = pathname[i++];	/* open bracket */
	  savej = j;
	  savei = i;
	  c = pathname[i++];	/* c == char after open bracket */
	  if (c == '^')		/* ignore pattern negation */
	    {
	      temp[j++] = c;
	      c = pathname[i++];
	    }
	  if (c == ']')		/* ignore right bracket if first char */
	    {
	      temp[j++] = c;
	      c = pathname[i++];
	    }
	  do
	    {
	      if (c == 0)
		goto endpat;
	      else if (c == CTLESC)
		{
		  /* skip c, check for EOS, let assignment at end of loop */
		  /* pathname[i] == backslash-escaped character */
		  if (pathname[i] == 0)
		    goto endpat;
		  temp[j++] = pathname[i++];
		}
	      else if (c == '[' && pathname[i] == ':')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  cclass = 1;
		}
	      else if (cclass && c == ':' && pathname[i] == ']')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  cclass = 0;
		}
	      else if (c == '[' && pathname[i] == '=')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  if (pathname[i] == ']')
		    temp[j++] = pathname[i++];		/* right brack can be in equiv */
		  equiv = 1;
		}
	      else if (equiv && c == '=' && pathname[i] == ']')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  equiv = 0;
		}
	      else if (c == '[' && pathname[i] == '.')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  if (pathname[i] == ']')
		    temp[j++] = pathname[i++];		/* right brack can be in collsym */
		  collsym = 1;
		}
	      else if (collsym && c == '.' && pathname[i] == ']')
		{
		  temp[j++] = c;
		  temp[j++] = pathname[i++];
		  collsym = 0;
		}
	      else
		temp[j++] = c;
	    }
	  while (((c = pathname[i++]) != ']') && c != 0);

	  /* If we don't find the closing bracket before we hit the end of
	     the string, rescan string without treating it as a bracket
	     expression (has implications for backslash and special ERE
	     chars) */
	  if (c == 0)
	    {
	      i = savei - 1;	/* -1 for autoincrement above */
	      j = savej;
	      continue;
	    }

	  temp[j++] = c;	/* closing right bracket */
	  i--;			/* increment will happen above in loop */
	  continue;		/* skip double assignment below */
	}
      else if (pathname[i] == '\\' && (qflags & QGLOB_REGEXP) == 0)
	{
	  /* XXX - if not quoting regexp, use backslash as quote char. Should
	     we just pass it through without treating it as special? That is
	     what ksh93 seems to do. */

	  /* If we want to pass through backslash unaltered, comment out these
	     lines. */
	  temp[j++] = '\\';

	  i++;
	  if (pathname[i] == '\0')
	    break;
	  /* If we are turning CTLESC CTLESC into CTLESC, we need to do that
	     even when the first CTLESC is preceded by a backslash. */
	  if ((qflags & QGLOB_CTLESC) && pathname[i] == CTLESC && (pathname[i+1] == CTLESC || pathname[i+1] == CTLNUL))
	    i++;	/* skip over the CTLESC */
	}
      else if (pathname[i] == '\\' && (qflags & QGLOB_REGEXP))
        last_was_backslash = 1;
      temp[j++] = pathname[i];
    }
endpat:
  temp[j] = '\0';

  return (temp);
}