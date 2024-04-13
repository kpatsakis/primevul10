std_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	JPEGState* sp = (JPEGState*) cinfo;

	if (num_bytes > 0) {
		if ((size_t)num_bytes > sp->src.bytes_in_buffer) {
			/* oops, buffer overrun */
			(void) std_fill_input_buffer(cinfo);
		} else {
			sp->src.next_input_byte += (size_t) num_bytes;
			sp->src.bytes_in_buffer -= (size_t) num_bytes;
		}
	}
}