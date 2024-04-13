test_decode_integer_end_of_input (xd3_stream *stream, int unused)
{
  return test_read_integer_error (stream, 1, "end-of-input in read_integer");
}