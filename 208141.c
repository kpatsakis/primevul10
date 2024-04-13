test_address_cache (xd3_stream *stream, int unused)
{
  int ret;
  usize_t i;
  usize_t offset;
  usize_t *addrs;
  uint8_t *big_buf, *buf_max;
  const uint8_t *buf;
  xd3_output *outp;
  uint8_t *modes;
  int mode_counts[16];

  stream->acache.s_near = stream->code_table_desc->near_modes;
  stream->acache.s_same = stream->code_table_desc->same_modes;

  if ((ret = xd3_encode_init_partial (stream))) { return ret; }

  addrs = (usize_t*) xd3_alloc (stream, sizeof (usize_t), ADDR_CACHE_ROUNDS);
  modes = (uint8_t*) xd3_alloc (stream, sizeof (uint8_t), ADDR_CACHE_ROUNDS);

  memset (mode_counts, 0, sizeof (mode_counts));
  memset (modes, 0, ADDR_CACHE_ROUNDS);

  addrs[0] = 0;

  mt_init (& static_mtrand, 0x9f73f7fc);

  /* First pass: encode addresses */
  xd3_init_cache (& stream->acache);

  for (offset = 1; offset < ADDR_CACHE_ROUNDS; offset += 1)
    {
      double p;
      usize_t addr;
      usize_t prev_i;
      usize_t nearby;

      p         = (mt_random (&static_mtrand) / (double)USIZE_T_MAX);
      prev_i    = mt_random (&static_mtrand) % offset;
      nearby    = (mt_random (&static_mtrand) % 256) % offset;
      nearby    = max (1U, nearby);

      if (p < 0.1)      { addr = addrs[offset-nearby]; }
      else if (p < 0.4) { addr = min (addrs[prev_i] + nearby, offset-1); }
      else              { addr = prev_i; }

      if ((ret = xd3_encode_address (stream, addr, offset, & modes[offset]))) { return ret; }

      addrs[offset] = addr;
      mode_counts[modes[offset]] += 1;
    }

  /* Copy addresses into a contiguous buffer. */
  big_buf = (uint8_t*) xd3_alloc (stream, xd3_sizeof_output (ADDR_HEAD (stream)), 1);

  for (offset = 0, outp = ADDR_HEAD (stream); outp != NULL; offset += outp->next, outp = outp->next_page)
    {
      memcpy (big_buf + offset, outp->base, outp->next);
    }

  buf_max = big_buf + offset;
  buf     = big_buf;

  /* Second pass: decode addresses */
  xd3_init_cache (& stream->acache);

  for (offset = 1; offset < ADDR_CACHE_ROUNDS; offset += 1)
    {
      uint32_t addr;

      if ((ret = xd3_decode_address (stream, offset, modes[offset], & buf, buf_max, & addr))) { return ret; }

      if (addr != addrs[offset])
	{
	  stream->msg = "incorrect decoded address";
	  return XD3_INTERNAL;
	}
    }

  /* Check that every byte, mode was used. */
  if (buf != buf_max)
    {
      stream->msg = "address bytes not used";
      return XD3_INTERNAL;
    }

  for (i = 0; i < (2 + stream->acache.s_same + stream->acache.s_near); i += 1)
    {
      if (mode_counts[i] == 0)
	{
	  stream->msg = "address mode not used";
	  return XD3_INTERNAL;
	}
    }

  xd3_free (stream, modes);
  xd3_free (stream, addrs);
  xd3_free (stream, big_buf);

  return 0;
}