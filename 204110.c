int fli_write_color_2(FILE *f, s_fli_header *fli_header, unsigned char *old_cmap, unsigned char *cmap)
{
	unsigned long chunkpos;
	unsigned short num_packets;
	s_fli_chunk chunk;
	chunkpos=ftell(f);
	fseek(f, chunkpos+8, SEEK_SET);
	num_packets=0;
	if (old_cmap==NULL) {
		unsigned short col_pos;
		num_packets=1;
		fli_write_char(f, 0); /* skip no color */
		fli_write_char(f, 0); /* 256 color */
		for (col_pos=0; col_pos<768; col_pos++) {
			fli_write_char(f, cmap[col_pos]);
		}
	} else {
		unsigned short cnt_skip, cnt_col, col_pos, col_start;
		col_pos=0;
		do {
			cnt_skip=0;
			while ((col_pos<256) && (old_cmap[col_pos*3+0]==cmap[col_pos*3+0]) && (old_cmap[col_pos*3+1]==cmap[col_pos*3+1]) && (old_cmap[col_pos*3+2]==cmap[col_pos*3+2])) {
				cnt_skip++; col_pos++;
			}
			col_start=col_pos*3;
			cnt_col=0;
			while ((col_pos<256) && !((old_cmap[col_pos*3+0]==cmap[col_pos*3+0]) && (old_cmap[col_pos*3+1]==cmap[col_pos*3+1]) && (old_cmap[col_pos*3+2]==cmap[col_pos*3+2]))) {
				cnt_col++; col_pos++;
			}
			if (cnt_col>0) {
				num_packets++;
				fli_write_char(f, cnt_skip);
				fli_write_char(f, cnt_col);
				while (cnt_col>0) {
					fli_write_char(f, cmap[col_start++]);
					fli_write_char(f, cmap[col_start++]);
					fli_write_char(f, cmap[col_start++]);
					cnt_col--;
				}
			}
		} while (col_pos<256);
	}

	if (num_packets>0) {
		chunk.size=ftell(f)-chunkpos;
		chunk.magic=FLI_COLOR_2;

		fseek(f, chunkpos, SEEK_SET);
		fli_write_long(f, chunk.size);
		fli_write_short(f, chunk.magic);
		fli_write_short(f, num_packets);

		if (chunk.size & 1) chunk.size++;
		fseek(f,chunkpos+chunk.size,SEEK_SET);
		return 1;
	}
	fseek(f,chunkpos, SEEK_SET);
	return 0;
}