parse_headers (mu_stream_t input, compose_env_t *env)
{
  int status;
  mu_header_t header;
  char *name = NULL;
  char *value = NULL;
  enum { STATE_INIT, STATE_READ, STATE_BODY } state = STATE_INIT;
  char *buf = NULL;
  size_t size = 0, n;
  int errcnt = 0, line = 0;
  
  if ((status = mu_header_create (&header, NULL, 0)) != 0)
    {
      mu_error (_("Cannot create header: %s"), mu_strerror (status));
      return parse_headers_fatal;
    }
  
  while (state != STATE_BODY &&
	 errcnt == 0 &&
	 mu_stream_getline (input, &buf, &size, &n) == 0 && n > 0)
    {
      mu_rtrim_class (buf, MU_CTYPE_SPACE);

      line++;
      switch (state)
	{
	case STATE_INIT:
	  if (!buf[0] || mu_isspace (buf[0]))
	    continue;
	  else
	    state = STATE_READ;
	  /*FALLTHRU*/
	  
	case STATE_READ:
	  if (buf[0] == 0)
	    state = STATE_BODY;
	  else if (mu_isspace (buf[0]))
	    {
	      /* A continuation line */
	      if (name)
		{
		  char *p = NULL;
		  mu_asprintf (&p, "%s\n%s", value, buf);
		  free (value);
		  value = p;
		}
	      else
		{
		  mu_error (_("%d: not a header line"), line);
		  errcnt++;
		}
	    }
	  else
	    {
	      char *p;
	      
	      if (name)
		{
		  mu_header_append (header, name, value[0] ? value : NULL);
		  free (name);
		  free (value);
		  name = value = NULL;
		}
	      p = strchr (buf, ':');
	      if (p)
		{
		  *p++ = 0;
		  while (*p && mu_isspace (*p))
		    p++;
		  value = mu_strdup (p);
		  name = mu_strdup (buf);
		}
	      else
		{
		  mu_error (_("%d: not a header line"), line);
		  errcnt++;
		}
	    }
	  break;
	  
	default:
	  abort ();
	}
    }
  
  free (buf);
  if (name)
    {
      mu_header_append (header, name, value);
      free (name);
      free (value);
    }     

  if (errcnt)
    {
      mu_header_destroy (&header);
      return parse_headers_error;
    }

  mu_header_destroy (&env->header);
  env->header = header;
  return parse_headers_ok;
}