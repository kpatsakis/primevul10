void fli_read_header(FILE *f, s_fli_header *fli_header)
{
	fli_header->filesize=fli_read_long(f);	/* 0 */
	fli_header->magic=fli_read_short(f);	/* 4 */
	fli_header->frames=fli_read_short(f);	/* 6 */
	fli_header->width=fli_read_short(f);	/* 8 */
	fli_header->height=fli_read_short(f);	/* 10 */
	fli_header->depth=fli_read_short(f);	/* 12 */
	fli_header->flags=fli_read_short(f);	/* 14 */
	if (fli_header->magic == HEADER_FLI) {
		/* FLI saves speed in 1/70s */
		fli_header->speed=fli_read_short(f)*14;		/* 16 */
	} else {
		if (fli_header->magic == HEADER_FLC) {
			/* FLC saves speed in 1/1000s */
			fli_header->speed=fli_read_long(f);	/* 16 */
		} else {
			fprintf(stderr, "error: magic number is wrong !\n");
			fli_header->magic = NO_HEADER;
		}
	}

	if (fli_header->width == 0)
	  fli_header->width = 320;

	if (fli_header->height == 0)
	  fli_header->height = 200;
}