sec_dist_func1 (xd3_stream *stream, xd3_output *data)
{
  int i, ret;
  for (i = 0; i < 100; i += 1)
    {
      if ((ret = xd3_emit_byte (stream, & data, 13))) { return ret; }
    }
  return 0;
}