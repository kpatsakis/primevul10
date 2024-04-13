void fli_read_copy(FILE *f, s_fli_header *fli_header, unsigned char *framebuf)
{
	fread(framebuf, fli_header->width, fli_header->height, f);
}