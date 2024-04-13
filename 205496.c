alloc_downsampled_buffers(TIFF* tif, jpeg_component_info* comp_info,
			  int num_components)
{
	JPEGState* sp = JState(tif);
	int ci;
	jpeg_component_info* compptr;
	JSAMPARRAY buf;
	int samples_per_clump = 0;

	for (ci = 0, compptr = comp_info; ci < num_components;
	     ci++, compptr++) {
		samples_per_clump += compptr->h_samp_factor *
			compptr->v_samp_factor;
		buf = TIFFjpeg_alloc_sarray(sp, JPOOL_IMAGE,
				compptr->width_in_blocks * DCTSIZE,
				(JDIMENSION) (compptr->v_samp_factor*DCTSIZE));
		if (buf == NULL)
			return (0);
		sp->ds_buffer[ci] = buf;
	}
	sp->samplesperclump = samples_per_clump;
	return (1);
}