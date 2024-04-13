pts_name (int fd, char **pts, size_t buf_len, struct stat64 *stp)
{
  int rv;
  char *buf = *pts;

  for (;;)
    {
      char *new_buf;

      if (buf_len)
	{
	  rv = __ptsname_internal (fd, buf, buf_len, stp);
	  if (rv != 0)
	    {
	      if (rv == ENOTTY)
		/* ptsname_r returns with ENOTTY to indicate
		   a descriptor not referring to a pty master.
		   For this condition, grantpt must return EINVAL.  */
		rv = EINVAL;
	      errno = rv;	/* Not necessarily set by __ptsname_r.  */
	      break;
	    }

	  if (memchr (buf, '\0', buf_len))
	    /* We succeeded and the returned name fit in the buffer.  */
	    break;

	  /* Try again with a longer buffer.  */
	  buf_len += buf_len;	/* Double it */
	}
      else
	/* No initial buffer; start out by mallocing one.  */
	buf_len = 128;		/* First time guess.  */

      if (buf != *pts)
	/* We've already malloced another buffer at least once.  */
	new_buf = (char *) realloc (buf, buf_len);
      else
	new_buf = (char *) malloc (buf_len);
      if (! new_buf)
	{
	  rv = -1;
	  __set_errno (ENOMEM);
	  break;
	}
      buf = new_buf;
    }

  if (rv == 0)
    *pts = buf;		/* Return buffer to the user.  */
  else if (buf != *pts)
    free (buf);		/* Free what we malloced when returning an error.  */

  return rv;
}