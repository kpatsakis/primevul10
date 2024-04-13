tables_term_destination(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;

	/* set tables length to number of bytes actually emitted */
	sp->jpegtables_length -= (uint32) sp->dest.free_in_buffer;
}