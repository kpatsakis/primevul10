sec_dist_func6 (xd3_stream *stream, xd3_output *data)
{
  int i, ret, x;
  for (i = 0; i < ALPHABET_SIZE*20; i += 1)
    {
      x = mt_random (&static_mtrand) % (ALPHABET_SIZE/2);
      if ((ret = xd3_emit_byte (stream, & data, x))) { return ret; }
    }
  return 0;
}