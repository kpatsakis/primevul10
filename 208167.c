sec_dist_func8 (xd3_stream *stream, xd3_output *data)
{
  int i, ret;
  for (i = 0; i < ALPHABET_SIZE*5; i += 1)
    {
      if ((ret = xd3_emit_byte (stream, & data, 0))) { return ret; }
      if ((ret = xd3_emit_byte (stream, & data, 64))) { return ret; }
      if ((ret = xd3_emit_byte (stream, & data, 128))) { return ret; }
      if ((ret = xd3_emit_byte (stream, & data, 255))) { return ret; }
    }
  return 0;
}