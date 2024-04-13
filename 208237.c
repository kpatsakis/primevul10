test_source_cksum_offset (xd3_stream *stream, int ignore)
{
  xd3_source source;

  // Inputs are:
  struct {
    xoff_t   cpos;   // stream->srcwin_cksum_pos;
    xoff_t   ipos;   // stream->total_in;
    xoff_t   size;   // stream->src->size;

    usize_t  input;  // input  32-bit offset
    xoff_t   output; // output 64-bit offset

  } cksum_test[] = {
    // If cpos is <= 2^32
    { 1, 1, 1, 1, 1 },

#if XD3_USE_LARGEFILE64
//    cpos            ipos            size            input         output
//    0x____xxxxxULL, 0x____xxxxxULL, 0x____xxxxxULL, 0x___xxxxxUL, 0x____xxxxxULL
    { 0x100100000ULL, 0x100000000ULL, 0x100200000ULL, 0x00000000UL, 0x100000000ULL },
    { 0x100100000ULL, 0x100000000ULL, 0x100200000ULL, 0xF0000000UL, 0x0F0000000ULL },

    { 0x100200000ULL, 0x100100000ULL, 0x100200000ULL, 0x00300000UL, 0x000300000ULL },

    { 25771983104ULL, 25770000000ULL, 26414808769ULL, 2139216707UL, 23614053187ULL },

#endif

    { 0, 0, 0, 0, 0 },
  }, *test_ptr;

  stream->src = &source;

  for (test_ptr = cksum_test; test_ptr->cpos; test_ptr++) {
	xoff_t r;
    stream->srcwin_cksum_pos = test_ptr->cpos;
    stream->total_in = test_ptr->ipos;

    r = xd3_source_cksum_offset(stream, test_ptr->input);
    CHECK(r == test_ptr->output);
  }
  return 0;
}