TIFFjpeg_alloc_sarray(JPEGState* sp, int pool_id,
		      JDIMENSION samplesperrow, JDIMENSION numrows)
{
	return CALLJPEG(sp, (JSAMPARRAY) NULL,
	    (*sp->cinfo.comm.mem->alloc_sarray)
		(&sp->cinfo.comm, pool_id, samplesperrow, numrows));
}