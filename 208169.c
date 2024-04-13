test_in_memory (xd3_stream *stream, int ignore)
{
  // test_text is 256 bytes
  uint8_t ibuf[sizeof(test_text)];
  uint8_t dbuf[sizeof(test_text)];
  uint8_t obuf[sizeof(test_text)];
  usize_t size = sizeof(test_text);
  usize_t dsize, osize;
  int r1, r2;
  int eflags = SECONDARY_DJW ? XD3_SEC_DJW : 0;

  memcpy(ibuf, test_text, size);
  memset(ibuf + 128, 0, 16);

  r1 = xd3_encode_memory(ibuf, size,
			 test_text, size,
			 dbuf, &dsize, size, eflags);

  r2 = xd3_decode_memory(dbuf, dsize,
			 test_text, size,
			 obuf, &osize, size, 0);

  if (r1 != 0 || r2 != 0 || dsize >= (size/2) || dsize < 1 ||
      osize != size) {
    stream->msg = "encode/decode size error";
    return XD3_INTERNAL;
  }

  if (memcmp(obuf, ibuf, size) != 0) {
    stream->msg = "encode/decode data error";
    return XD3_INTERNAL;
  }

  return 0;
}