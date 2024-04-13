tables_empty_output_buffer(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;
	void* newbuf;

	/* the entire buffer has been filled; enlarge it by 1000 bytes */
	newbuf = _TIFFrealloc((void*) sp->jpegtables,
			      (tmsize_t) (sp->jpegtables_length + 1000));
	if (newbuf == NULL)
		ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 100);
	sp->dest.next_output_byte = (JOCTET*) newbuf + sp->jpegtables_length;
	sp->dest.free_in_buffer = (size_t) 1000;
	sp->jpegtables = newbuf;
	sp->jpegtables_length += 1000;
	return (TRUE);
}