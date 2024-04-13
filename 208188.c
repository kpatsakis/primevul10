test_encode_decode_uint32_t (xd3_stream *stream, int unused)
{
  TEST_ENCODE_DECODE_INTEGER(uint32_t,1U,UINT32_MAX);
}