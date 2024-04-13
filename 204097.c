void fli_read_black(FILE *f, s_fli_header *fli_header, unsigned char *framebuf)
{
	memset(framebuf, 0, fli_header->width * fli_header->height);
}