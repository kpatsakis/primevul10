sec_dist_func10 (xd3_stream *stream, xd3_output *data)
{
  int i, j, ret;
  for (i = 0; i < ALPHABET_SIZE; i += 1)
    {
      for (j = 0; j <= (i*i); j += 1)
	{
	  if ((ret = xd3_emit_byte (stream, & data, i))) { return ret; }
	}
    }
  return 0;
}