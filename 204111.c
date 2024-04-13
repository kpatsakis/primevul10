void fli_read_frame(FILE *f, s_fli_header *fli_header, unsigned char *old_framebuf, unsigned char *old_cmap, unsigned char *framebuf, unsigned char *cmap)
{
	s_fli_frame fli_frame;
	unsigned long framepos;
	int c;
	framepos=ftell(f);

	fli_frame.size=fli_read_long(f);
	fli_frame.magic=fli_read_short(f);
	fli_frame.chunks=fli_read_short(f);

	if (fli_frame.magic == FRAME) {
		fseek(f, framepos+16, SEEK_SET);
		for (c=0;c<fli_frame.chunks;c++) {
			s_fli_chunk chunk;
			unsigned long chunkpos;
			chunkpos = ftell(f);
			chunk.size=fli_read_long(f);
			chunk.magic=fli_read_short(f);
			switch (chunk.magic) {
				case FLI_COLOR:   fli_read_color(f, fli_header, old_cmap, cmap); break;
				case FLI_COLOR_2: fli_read_color_2(f, fli_header, old_cmap, cmap); break;
				case FLI_BLACK:	  fli_read_black(f, fli_header, framebuf); break;
				case FLI_BRUN:    fli_read_brun(f, fli_header, framebuf); break;
				case FLI_COPY:    fli_read_copy(f, fli_header, framebuf); break;
				case FLI_LC:      fli_read_lc(f, fli_header, old_framebuf, framebuf); break;
				case FLI_LC_2:    fli_read_lc_2(f, fli_header, old_framebuf, framebuf); break;
				case FLI_MINI:	  /* unused, skip */ break;
				default: /* unknown, skip */ break;
			}
			if (chunk.size & 1) chunk.size++;
			fseek(f,chunkpos+chunk.size,SEEK_SET);
		}
	} /* else: unknown, skip */
	fseek(f, framepos+fli_frame.size, SEEK_SET);
}