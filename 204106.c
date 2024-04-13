void fli_read_lc(FILE *f, s_fli_header *fli_header, unsigned char *old_framebuf, unsigned char *framebuf)
{
	unsigned short yc, firstline, numline;
	unsigned char *pos;
	memcpy(framebuf, old_framebuf, fli_header->width * fli_header->height);
	firstline = fli_read_short(f);
	numline = fli_read_short(f);
	if (numline > fli_header->height || fli_header->height-numline < firstline)
		return;

	for (yc=0; yc < numline; yc++) {
		unsigned short pc, pcnt;
		size_t n, xc;
		pc=fli_read_char(f);
		xc=0;
		pos=framebuf+(fli_header->width * (firstline+yc));
		n=(size_t)fli_header->width * (fli_header->height-firstline-yc);
		for (pcnt=pc; pcnt>0; pcnt--) {
			unsigned short ps,skip;
			skip=fli_read_char(f);
			ps=fli_read_char(f);
			xc+=MIN(n-xc,skip);
			if (ps & 0x80) {
				unsigned char val;
				size_t len;
				ps=-(signed char)ps;
				val=fli_read_char(f);
				len=MIN(n-xc,ps);
				memset(&(pos[xc]), val, len);
				xc+=len;
			} else {
				size_t len;
				len=MIN(n-xc,ps);
				fread(&(pos[xc]), len, 1, f);
				xc+=len;
			}
		}
	}
}