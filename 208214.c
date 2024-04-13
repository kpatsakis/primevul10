sec_dist_func4 (xd3_stream *stream, xd3_output *data)
{
  int i, ret, x;
  for (i = 0; i < ALPHABET_SIZE*20; i += 1)
    {
      x = mt_exp_rand (10, ALPHABET_SIZE/2);
      if ((ret = xd3_emit_byte (stream, & data, x))) { return ret; }
    }
  return 0;
}