tables_init_source(j_decompress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;

	sp->src.next_input_byte = (const JOCTET*) sp->jpegtables;
	sp->src.bytes_in_buffer = (size_t) sp->jpegtables_length;
}