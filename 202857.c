parse_c_string (char const *s, char const **endp)
{
  char *u, *v;

  assert (*s++ == '"');
  u = v = xmalloc (strlen (s));
  for (;;)
    {
      char c = *s++;

      switch (c)
	{
	  case 0:
	    goto fail;

	  case '"':
	    *v++ = 0;
	    v = realloc (u, v - u);
	    if (v)
	      u = v;
	    if (endp)
	      *endp = s;
	    return u;

	  case '\\':
	    break;

	  default:
	    *v++ = c;
	    continue;
	}

      c = *s++;
      switch (c)
	{
	  case 'a': c = '\a'; break;
	  case 'b': c = '\b'; break;
	  case 'f': c = '\f'; break;
	  case 'n': c = '\n'; break;
	  case 'r': c = '\r'; break;
	  case 't': c = '\t'; break;
	  case 'v': c = '\v'; break;
	  case '\\': case '"':
	    break;  /* verbatim */
	  case '0': case '1': case '2': case '3':
	    {
	      int acc = (c - '0') << 6;

	      c = *s++;
	      if (c < '0' || c > '7')
	        goto fail;
	      acc |= (c - '0') << 3;
	      c = *s++;
	      if (c < '0' || c > '7')
	        goto fail;
	      acc |= (c - '0');
	      c = acc;
	      break;
	    }
	  default:
	    goto fail;
	}
      *v++ = c;
    }

fail:
  free (u);
  if (endp)
    *endp = s;
  return NULL;
}