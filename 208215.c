test_forward_match (xd3_stream *stream, int unused)
{
  usize_t i;
  uint8_t buf1[256], buf2[256];

  memset(buf1, 0, 256);
  memset(buf2, 0, 256);

  for (i = 0; i < 256; i++)
    {
      CHECK(xd3_forward_match(buf1, buf2, i) == (int)i);
    }

  for (i = 0; i < 255; i++)
    {
      buf2[i] = 1;
      CHECK(xd3_forward_match(buf1, buf2, 256) == (int)i);
      buf2[i] = 0;
    }

  return 0;
}