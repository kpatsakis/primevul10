bash_forward_shellword (count, key)
     int count, key;
{
  size_t slen;
  int c, p;
  DECLARE_MBSTATE;

  if (count < 0)
    return (bash_backward_shellword (-count, key));

  /* The tricky part of this is deciding whether or not the first character
     we're on is an unquoted metacharacter.  Not completely handled yet. */
  /* XXX - need to test this stuff with backslash-escaped shell
     metacharacters and unclosed single- and double-quoted strings. */

  p = rl_point;
  slen = rl_end;

  while (count)
    {
      if (p == rl_end)
	{
	  rl_point = rl_end;
	  return 0;
	}

      /* Are we in a quoted string?  If we are, move to the end of the quoted
         string and continue the outer loop. We only want quoted strings, not
         backslash-escaped characters, but char_is_quoted doesn't
         differentiate. */
      if (char_is_quoted (rl_line_buffer, p) && p > 0 && rl_line_buffer[p-1] != '\\')
	{
	  do
	    ADVANCE_CHAR (rl_line_buffer, slen, p);
	  while (p < rl_end && char_is_quoted (rl_line_buffer, p));
	  count--;
	  continue;
	}

      /* Rest of code assumes we are not in a quoted string. */
      /* Move forward until we hit a non-metacharacter. */
      while (p < rl_end && (c = rl_line_buffer[p]) && WORDDELIM (c))
	{
	  switch (c)
	    {
	    default:
	      ADVANCE_CHAR (rl_line_buffer, slen, p);
	      continue;		/* straight back to loop, don't increment p */
	    case '\\':
	      if (p < rl_end && rl_line_buffer[p])
		ADVANCE_CHAR (rl_line_buffer, slen, p);
	      break;
	    case '\'':
	      p = skip_to_delim (rl_line_buffer, ++p, "'", SD_NOJMP);
	      break;
	    case '"':
	      p = skip_to_delim (rl_line_buffer, ++p, "\"", SD_NOJMP);
	      break;
	    }

	  if (p < rl_end)
	    p++;
	}

      if (rl_line_buffer[p] == 0 || p == rl_end)
        {
	  rl_point = rl_end;
	  rl_ding ();
	  return 0;
        }
	
      /* Now move forward until we hit a non-quoted metacharacter or EOL */
      while (p < rl_end && (c = rl_line_buffer[p]) && WORDDELIM (c) == 0)
	{
	  switch (c)
	    {
	    default:
	      ADVANCE_CHAR (rl_line_buffer, slen, p);
	      continue;		/* straight back to loop, don't increment p */
	    case '\\':
	      if (p < rl_end && rl_line_buffer[p])
		ADVANCE_CHAR (rl_line_buffer, slen, p);
	      break;
	    case '\'':
	      p = skip_to_delim (rl_line_buffer, ++p, "'", SD_NOJMP);
	      break;
	    case '"':
	      p = skip_to_delim (rl_line_buffer, ++p, "\"", SD_NOJMP);
	      break;
	    }

	  if (p < rl_end)
	    p++;
	}

      if (p == rl_end || rl_line_buffer[p] == 0)
	{
	  rl_point = rl_end;
	  return (0);
	}

      count--;      
    }

  rl_point = p;
  return (0);
}