tables_init_destination(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;

	/* while building, jpegtables_length is allocated buffer size */
	sp->dest.next_output_byte = (JOCTET*) sp->jpegtables;
	sp->dest.free_in_buffer = (size_t) sp->jpegtables_length;
}