test_secondary (xd3_stream *stream, const xd3_sec_type *sec, usize_t groups)
{
  usize_t test_i;
  int ret;
  xd3_output *in_head, *out_head, *p;
  usize_t p_off, input_size, compress_size;
  uint8_t *dec_input = NULL, *dec_output = NULL, *dec_correct = NULL;
  xd3_sec_stream *enc_stream;
  xd3_sec_cfg cfg;

  memset (& cfg, 0, sizeof (cfg));

  cfg.inefficient = 1;

  for (cfg.ngroups = 1; cfg.ngroups <= groups; cfg.ngroups += 1)
    {
      XPR(NTR "\n...");
      for (test_i = 0; test_i < SIZEOF_ARRAY (sec_dists); test_i += 1)
	{
	  mt_init (& static_mtrand, 0x9f73f7fc);

	  in_head  = xd3_alloc_output (stream, NULL);
	  out_head = xd3_alloc_output (stream, NULL);
	  enc_stream = sec->alloc (stream);
	  dec_input = NULL;
	  dec_output = NULL;
	  dec_correct = NULL;

	  if (in_head == NULL || out_head == NULL || enc_stream == NULL)
	    {
	      goto nomem;
	    }

	  if ((ret = sec_dists[test_i] (stream, in_head))) { goto fail; }

	  if ((ret = sec->init (stream, enc_stream, 1)) != 0) { goto fail; }

	  /* Encode data */
	  if ((ret = sec->encode (stream, enc_stream,
				  in_head, out_head, & cfg)))
	    {
	      XPR(NT "test %u: encode: %s", test_i, stream->msg);
	      goto fail;
	    }

	  /* Calculate sizes, allocate contiguous arrays for decoding */
	  input_size    = xd3_sizeof_output (in_head);
	  compress_size = xd3_sizeof_output (out_head);

	  XPR(NTR "%.3f", 8.0 * (double) compress_size / (double) input_size);

	  if ((dec_input   = (uint8_t*) xd3_alloc (stream, compress_size, 1)) == NULL ||
	      (dec_output  = (uint8_t*) xd3_alloc (stream, input_size, 1)) == NULL ||
	      (dec_correct = (uint8_t*) xd3_alloc (stream, input_size, 1)) == NULL)
	    {
	      goto nomem;
	    }

	  /* Fill the compressed data array */
	  for (p_off = 0, p = out_head; p != NULL;
	       p_off += p->next, p = p->next_page)
	    {
	      memcpy (dec_input + p_off, p->base, p->next);
	    }

	  CHECK(p_off == compress_size);

	  /* Fill the input data array */
	  for (p_off = 0, p = in_head; p != NULL;
	       p_off += p->next, p = p->next_page)
	    {
	      memcpy (dec_correct + p_off, p->base, p->next);
	    }

	  CHECK(p_off == input_size);

	  if ((ret = test_secondary_decode (stream, sec, input_size,
					    compress_size, dec_input,
					    dec_correct, dec_output)))
	    {
	      XPR(NT "test %u: decode: %s", test_i, stream->msg);
	      goto fail;
	    }

	  /* Single-bit error test, only cover the first 10 bytes.
	   * Some non-failures are expected in the Huffman case:
	   * Changing the clclen array, for example, may not harm the
	   * decoding.  Really looking for faults here. */
	  {
	    int i;
	    int bytes = min (compress_size, 10U);
	    for (i = 0; i < bytes * 8; i += 1)
	      {
		dec_input[i/8] ^= 1 << (i%8);

		if ((ret = test_secondary_decode (stream, sec, input_size,
						  compress_size, dec_input,
						  dec_correct, dec_output))
		    == 0)
		  {
		    /*XPR(NT "test %u: decode single-bit [%u/%u]
		      error non-failure", test_i, i/8, i%8);*/
		  }

		dec_input[i/8] ^= 1 << (i%8);

		if ((i % (2*bytes)) == (2*bytes)-1)
		  {
		    DOT ();
		  }
	      }
	    ret = 0;
	  }

	  if (0) { nomem: ret = ENOMEM; }

	fail:
	  sec->destroy (stream, enc_stream);
	  xd3_free_output (stream, in_head);
	  xd3_free_output (stream, out_head);
	  xd3_free (stream, dec_input);
	  xd3_free (stream, dec_output);
	  xd3_free (stream, dec_correct);

	  if (ret != 0) { return ret; }
	}
    }

  return 0;
}