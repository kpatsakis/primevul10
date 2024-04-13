void fli_write_header(FILE *f, s_fli_header *fli_header)
{
	fli_header->filesize=ftell(f);
	fseek(f, 0, SEEK_SET);
	fli_write_long(f, fli_header->filesize);	/* 0 */
	fli_write_short(f, fli_header->magic);	/* 4 */
	fli_write_short(f, fli_header->frames);	/* 6 */
	fli_write_short(f, fli_header->width);	/* 8 */
	fli_write_short(f, fli_header->height);	/* 10 */
	fli_write_short(f, fli_header->depth);	/* 12 */
	fli_write_short(f, fli_header->flags);	/* 14 */
	if (fli_header->magic == HEADER_FLI) {
		/* FLI saves speed in 1/70s */
		fli_write_short(f, fli_header->speed / 14);	/* 16 */
	} else {
		if (fli_header->magic == HEADER_FLC) {
			/* FLC saves speed in 1/1000s */
			fli_write_long(f, fli_header->speed);	/* 16 */
			fseek(f, 80, SEEK_SET);
			fli_write_long(f, fli_header->oframe1);	/* 80 */
			fli_write_long(f, fli_header->oframe2);	/* 84 */
		} else {
			fprintf(stderr, "error: magic number in header is wrong !\n");
		}
	}
}