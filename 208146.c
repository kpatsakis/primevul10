sec_dist_func2 (xd3_stream *stream, xd3_output *data)
{
  int i, ret;
  for (i = 0; i < ALPHABET_SIZE; i += 1)
    {
      if ((ret = xd3_emit_byte (stream, & data, i%(ALPHABET_SIZE/2)))) { return ret; }
    }
  return 0;
}