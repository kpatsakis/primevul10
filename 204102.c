void fli_write_brun(FILE *f, s_fli_header *fli_header, unsigned char *framebuf)
{
	unsigned long chunkpos;
	s_fli_chunk chunk;
	unsigned short yc;
	unsigned char *linebuf;

	chunkpos=ftell(f);
	fseek(f, chunkpos+6, SEEK_SET);

	for (yc=0; yc < fli_header->height; yc++) {
		unsigned short xc, t1, pc, tc;
		unsigned long linepos, lineend, bc;
		linepos=ftell(f); bc=0;
		fseek(f, 1, SEEK_CUR);
		linebuf=framebuf + (yc*fli_header->width);
		xc=0; tc=0; t1=0;
		while (xc < fli_header->width) {
			pc=1;
			while ((pc<120) && ((xc+pc)<fli_header->width) && (linebuf[xc+pc] == linebuf[xc])) {
				pc++;
			}
			if (pc>2) {
				if (tc>0) {
					bc++;
					fli_write_char(f, (tc-1)^0xFF);
					fwrite(linebuf+t1, 1, tc, f);
					tc=0;
				}
				bc++;
				fli_write_char(f, pc);
				fli_write_char(f, linebuf[xc]);
				t1=xc+pc;
			} else {
				tc+=pc;
				if (tc>120) {
					bc++;
					fli_write_char(f, (tc-1)^0xFF);
					fwrite(linebuf+t1, 1, tc, f);
					tc=0;
					t1=xc+pc;
				}
			}
			xc+=pc;
		}
		if (tc>0) {
			bc++;
			fli_write_char(f, (tc-1)^0xFF);
			fwrite(linebuf+t1, 1, tc, f);
			tc=0;
		}
		lineend=ftell(f);
		fseek(f, linepos, SEEK_SET);
		fli_write_char(f, bc);
		fseek(f, lineend, SEEK_SET);
	}

	chunk.size=ftell(f)-chunkpos;
	chunk.magic=FLI_BRUN;

	fseek(f, chunkpos, SEEK_SET);
	fli_write_long(f, chunk.size);
	fli_write_short(f, chunk.magic);

	if (chunk.size & 1) chunk.size++;
	fseek(f,chunkpos+chunk.size,SEEK_SET);
}