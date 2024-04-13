test_encode_decode_uint64_t (xd3_stream *stream, int unused)
{
  TEST_ENCODE_DECODE_INTEGER(uint64_t,1ULL,UINT64_MAX);
}