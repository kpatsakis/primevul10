test_decode_integer_overflow (xd3_stream *stream, int unused)
{
  return test_read_integer_error (stream, 0, "overflow in read_intger");
}