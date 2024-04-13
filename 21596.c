bash_backward_shellword (count, key)
     int count, key;
{
  size_t slen;
  int c, p, prev_p;
  DECLARE_MBSTATE;

  if (count < 0)
    return (bash_forward_shellword (-count, key));

  p = rl_point;
  slen = rl_end;

  while (count)
    {
      if (p == 0)
	{
	  rl_point = 0;
	  return 0;
	}

      /* Move backward until we hit a non-metacharacter. We want to deal
	 with the characters before point, so we move off a word if we're
	 at its first character. */
      BACKUP_CHAR (rl_line_buffer, slen, p);
      while (p > 0)
	{
	  c = rl_line_buffer[p];
	  if (WORDDELIM (c) == 0 || char_is_quoted (rl_line_buffer, p))
	    break;
	  BACKUP_CHAR (rl_line_buffer, slen, p);
	}

      if (p == 0)
	{
	  rl_point = 0;
	  return 0;
	}

      /* Now move backward until we hit a metacharacter or BOL. Leave point
	 at the start of the shellword or at BOL. */
      prev_p = p;
      while (p > 0)
	{
	  c = rl_line_buffer[p];
	  if (WORDDELIM (c) && char_is_quoted (rl_line_buffer, p) == 0)
	    {
	      p = prev_p;
	      break;
	    }
	  prev_p = p;
	  BACKUP_CHAR (rl_line_buffer, slen, p);
	}

      count--;
    }

  rl_point = p;
  return 0;
}