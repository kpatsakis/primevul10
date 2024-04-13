test_make_inputs (xd3_stream *stream, xoff_t *ss_out, xoff_t *ts_out)
{
  usize_t ts = (mt_random (&static_mtrand) % TEST_FILE_MEAN) +
    TEST_FILE_MEAN / 2;
  usize_t ss = (mt_random (&static_mtrand) % TEST_FILE_MEAN) +
    TEST_FILE_MEAN / 2;
  uint8_t *buf = (uint8_t*) malloc (ts + ss), *sbuf = buf, *tbuf = buf + ss;
  usize_t sadd = 0, sadd_max = (usize_t)(ss * TEST_ADD_RATIO);
  FILE  *tf = NULL, *sf = NULL;
  usize_t i, j;
  int ret;

  if (buf == NULL) { return ENOMEM; }

  if ((tf = fopen (TEST_TARGET_FILE, "w")) == NULL ||
      (ss_out != NULL && (sf = fopen (TEST_SOURCE_FILE, "w")) == NULL))
    {
      stream->msg = "write failed";
      ret = get_errno ();
      goto failure;
    }

  if (ss_out != NULL)
    {
      for (i = 0; i < ss; )
	{
	  sbuf[i++] = (uint8_t) mt_random (&static_mtrand);
	}
    }

  /* Then modify the data to produce copies, everything not copied is
   * an add.  The following logic produces the TEST_ADD_RATIO.  The
   * variable SADD contains the number of adds so far, which should
   * not exceed SADD_MAX. */

  /* XPR(NT "ss = %u ts = %u\n", ss, ts); */
  for (i = 0; i < ts; )
    {
      usize_t left = ts - i;
      usize_t next = mt_exp_rand ((uint32_t) TEST_ADD_MEAN,
				  (uint32_t) TEST_ADD_MAX);
      usize_t add_left = sadd_max - sadd;
      double add_prob = (left == 0) ? 0 : (add_left / (double) left);
      int do_copy;

      next = min (left, next);
      do_copy = (next > add_left ||
		 (mt_random (&static_mtrand) / \
		  (double)USIZE_T_MAX) >= add_prob);

      if (ss_out == NULL)
	{
	  do_copy &= (i > 0);
	}
      else
	{
	  do_copy &= (ss - next) > 0;
	}

      if (do_copy)
	{
	  /* Copy */
	  size_t offset = mt_random (&static_mtrand) % ((ss_out == NULL) ?
							i :
							(ss - next));
	  /* XPR(NT "[%u] copy %u at %u ", i, next, offset); */

	  for (j = 0; j < next; j += 1)
	    {
	      char c = ((ss_out == NULL) ? tbuf : sbuf)[offset + j];
	      /* XPR(NT "%x%x", (c >> 4) & 0xf, c & 0xf); */
	      tbuf[i++] = c;
	    }
	  /* XPR(NT "\n"); */
	}
      else
	{
	  /* Add */
	  /* XPR(NT "[%u] add %u ", i, next); */
	  for (j = 0; j < next; j += 1)
	    {
	      char c = (char) mt_random (&static_mtrand);
	      /* XPR(NT "%x%x", (c >> 4) & 0xf, c & 0xf); */
	      tbuf[i++] = c;
	    }
	  /* XPR(NT "\n"); */
	  sadd += next;
	}
    }

  /* XPR(NT "sadd = %u max = %u\n", sadd, sadd_max); */

  if ((fwrite (tbuf, 1, ts, tf) != ts) ||
      (ss_out != NULL && (fwrite (sbuf, 1, ss, sf) != ss)))
    {
      stream->msg = "write failed";
      ret = get_errno ();
      goto failure;
    }

  if ((ret = fclose (tf)) || (ss_out != NULL && (ret = fclose (sf))))
    {
      stream->msg = "close failed";
      ret = get_errno ();
      goto failure;
    }

  if (ts_out) { (*ts_out) = ts; }
  if (ss_out) { (*ss_out) = ss; }

 failure:
  free (buf);
  return ret;
}