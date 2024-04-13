void fli_write_black(FILE *f, s_fli_header *fli_header, unsigned char *framebuf)
{
	s_fli_chunk chunk;

	chunk.size=6;
	chunk.magic=FLI_BLACK;

	fli_write_long(f, chunk.size);
	fli_write_short(f, chunk.magic);
}