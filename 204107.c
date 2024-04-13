void fli_write_copy(FILE *f, s_fli_header *fli_header, unsigned char *framebuf)
{

	unsigned long chunkpos;
	s_fli_chunk chunk;
	chunkpos=ftell(f);
	fseek(f, chunkpos+6, SEEK_SET);
	fwrite(framebuf, fli_header->width, fli_header->height, f);
	chunk.size=ftell(f)-chunkpos;
	chunk.magic=FLI_COPY;

	fseek(f, chunkpos, SEEK_SET);
	fli_write_long(f, chunk.size);
	fli_write_short(f, chunk.magic);

	if (chunk.size & 1) chunk.size++;
	fseek(f,chunkpos+chunk.size,SEEK_SET);
}