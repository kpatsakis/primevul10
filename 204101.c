void fli_write_frame(FILE *f, s_fli_header *fli_header, unsigned char *old_framebuf, unsigned char *old_cmap, unsigned char *framebuf, unsigned char *cmap, unsigned short codec_mask)
{
	s_fli_frame fli_frame;
	unsigned long framepos, frameend;
	framepos=ftell(f);
	fseek(f, framepos+16, SEEK_SET);

	switch (fli_header->frames) {
		case 0: fli_header->oframe1=framepos; break;
		case 1: fli_header->oframe2=framepos; break;
	}

	fli_frame.size=0;
	fli_frame.magic=FRAME;
	fli_frame.chunks=0;

	/*
	 * create color chunk
	 */
	if (fli_header->magic == HEADER_FLI) {
		if (fli_write_color(f, fli_header, old_cmap, cmap)) fli_frame.chunks++;
	} else {
		if (fli_header->magic == HEADER_FLC) {
			if (fli_write_color_2(f, fli_header, old_cmap, cmap)) fli_frame.chunks++;
		} else {
			fprintf(stderr, "error: magic number in header is wrong !\n");
		}
	}

#if 0
	if (codec_mask & W_COLOR) {
		if (fli_write_color(f, fli_header, old_cmap, cmap)) fli_frame.chunks++;
	}
	if (codec_mask & W_COLOR_2) {
		if (fli_write_color_2(f, fli_header, old_cmap, cmap)) fli_frame.chunks++;
	}
#endif
	/* create bitmap chunk */
	if (old_framebuf==NULL) {
		fli_write_brun(f, fli_header, framebuf);
	} else {
		fli_write_lc(f, fli_header, old_framebuf, framebuf);
	}
	fli_frame.chunks++;

	frameend=ftell(f);
	fli_frame.size=frameend-framepos;
	fseek(f, framepos, SEEK_SET);
	fli_write_long(f, fli_frame.size);
	fli_write_short(f, fli_frame.magic);
	fli_write_short(f, fli_frame.chunks);
	fseek(f, frameend, SEEK_SET);
	fli_header->frames++;
}