sec_dist_func9 (xd3_stream *stream, xd3_output *data)
{
  int i, ret;

  int ramp   = 0;
  int rcount = 0;
  int prom   = 0;
  int pcount = 0;

  /* 200 was long enough to trigger it--only when stricter checking
   * that counted all blocks was turned on, but it seems I deleted
   * this code. (missing fgk_free_block on line 398). */
  for (i = 0; i < ALPHABET_SIZE*200; i += 1)
    {
    repeat:
      if (ramp < ALPHABET_SIZE)
	{
	  /* Initially Nth symbol has (N+1) frequency */
	  if (rcount <= ramp)
	    {
	      rcount += 1;
	      if ((ret = xd3_emit_byte (stream, & data, ramp))) { return ret; }
	      continue;
	    }

	  ramp   += 1;
	  rcount  = 0;
	  goto repeat;
	}

      /* Thereafter, promote least freq to max freq */
      if (pcount == ALPHABET_SIZE)
	{
	  pcount = 0;
	  prom   = (prom + 1) % ALPHABET_SIZE;
	}

      pcount += 1;
      if ((ret = xd3_emit_byte (stream, & data, prom))) { return ret; }
    }

  return 0;
}