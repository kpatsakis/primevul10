sec_dist_func11 (xd3_stream *stream, xd3_output *data)
{
  int sum0 = 0;
  int sum1 = 1;
  int i, j, ret;
  for (i = 0; i < 33; ++i)
    {
      for (j = 0; j < (sum0 + sum1); ++j)
	{
	  if ((ret = xd3_emit_byte (stream, & data, i))) { return ret; }
	}
      sum0 = sum1;
      sum1 = j;
    }
  return 0;
}