void fli_write_lc(FILE *f, s_fli_header *fli_header, unsigned char *old_framebuf, unsigned char *framebuf)
{
	unsigned long chunkpos;
	s_fli_chunk chunk;
	unsigned short yc, firstline, numline, lastline;
	unsigned char *linebuf, *old_linebuf;

	chunkpos=ftell(f);
	fseek(f, chunkpos+6, SEEK_SET);

	/* first check, how many lines are unchanged at the beginning */
	firstline=0;
	while ((memcmp(old_framebuf+(firstline*fli_header->width), framebuf+(firstline*fli_header->width), fli_header->width)==0) && (firstline<fli_header->height)) firstline++;

	/* then check from the end, how many lines are unchanged */
	if (firstline<fli_header->height) {
		lastline=fli_header->height-1;
		while ((memcmp(old_framebuf+(lastline*fli_header->width), framebuf+(lastline*fli_header->width), fli_header->width)==0) && (lastline>firstline)) lastline--;
		numline=(lastline-firstline)+1;
	} else {
		numline=0;
	}
	if (numline==0) firstline=0;

	fli_write_short(f, firstline);
	fli_write_short(f, numline);

	for (yc=0; yc < numline; yc++) {
		unsigned short xc, sc, cc, tc;
		unsigned long linepos, lineend, bc;
		linepos=ftell(f); bc=0;
		fseek(f, 1, SEEK_CUR);

		linebuf=framebuf + ((firstline+yc)*fli_header->width);
		old_linebuf=old_framebuf + ((firstline+yc)*fli_header->width);
		xc=0;
		while (xc < fli_header->width) {
			sc=0;
			while ((linebuf[xc]==old_linebuf[xc]) && (xc<fli_header->width) && (sc<255)) {
				xc++; sc++;
			}
			fli_write_char(f, sc);
			cc=1;
			while ((linebuf[xc]==linebuf[xc+cc]) && ((xc+cc)<fli_header->width) && (cc<120)) {
				cc++;
			}
			if (cc>2) {
				bc++;
				fli_write_char(f, (cc-1)^0xFF);
				fli_write_char(f, linebuf[xc]);
				xc+=cc;
			} else {
				tc=0;
				do {
					sc=0;
					while ((linebuf[tc+xc+sc]==old_linebuf[tc+xc+sc]) && ((tc+xc+sc)<fli_header->width) && (sc<5)) {
						sc++;
					}
					cc=1;
					while ((linebuf[tc+xc]==linebuf[tc+xc+cc]) && ((tc+xc+cc)<fli_header->width) && (cc<10)) {
						cc++;
					}
					tc++;
				} while ((tc<120) && (cc<9) && (sc<4) && ((xc+tc)<fli_header->width));
				bc++;
				fli_write_char(f, tc);
				fwrite(linebuf+xc, tc, 1, f);
				xc+=tc;
			}
		}
		lineend=ftell(f);
		fseek(f, linepos, SEEK_SET);
		fli_write_char(f, bc);
		fseek(f, lineend, SEEK_SET);
	}

	chunk.size=ftell(f)-chunkpos;
	chunk.magic=FLI_LC;

	fseek(f, chunkpos, SEEK_SET);
	fli_write_long(f, chunk.size);
	fli_write_short(f, chunk.magic);

	if (chunk.size & 1) chunk.size++;
	fseek(f,chunkpos+chunk.size,SEEK_SET);
}