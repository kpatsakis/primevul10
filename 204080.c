write_data (FILE *out, FILE *out2, const char *buf, int bufsize,
            wgint *skip, wgint *written)
{
  if (out == NULL && out2 == NULL)
    return 1;
  if (*skip > bufsize)
    {
      *skip -= bufsize;
      return 1;
    }
  if (*skip)
    {
      buf += *skip;
      bufsize -= *skip;
      *skip = 0;
      if (bufsize == 0)
        return 1;
    }

  if (out != NULL)
    fwrite (buf, 1, bufsize, out);
  if (out2 != NULL)
    fwrite (buf, 1, bufsize, out2);
  *written += bufsize;

  /* Immediately flush the downloaded data.  This should not hinder
     performance: fast downloads will arrive in large 16K chunks
     (which stdio would write out immediately anyway), and slow
     downloads wouldn't be limited by disk speed.  */

  /* 2005-04-20 SMS.
     Perhaps it shouldn't hinder performance, but it sure does, at least
     on VMS (more than 2X).  Rather than speculate on what it should or
     shouldn't do, it might make more sense to test it.  Even better, it
     might be nice to explain what possible benefit it could offer, as
     it appears to be a clear invitation to poor performance with no
     actual justification.  (Also, why 16K?  Anyone test other values?)
  */
#ifndef __VMS
  if (out != NULL)
    fflush (out);
  if (out2 != NULL)
    fflush (out2);
#endif /* ndef __VMS */
  if (out != NULL && ferror (out))
    return -1;
  else if (out2 != NULL && ferror (out2))
    return -2;
  else
    return 0;
}