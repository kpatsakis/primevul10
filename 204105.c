void fli_read_color_2(FILE *f, s_fli_header *fli_header, unsigned char *old_cmap, unsigned char *cmap)
{
	unsigned short num_packets, cnt_packets, col_pos;
	num_packets=fli_read_short(f);
	col_pos=0;
	for (cnt_packets=num_packets; cnt_packets>0; cnt_packets--) {
		unsigned short skip_col, num_col, col_cnt;
		skip_col=fli_read_char(f);
		num_col=fli_read_char(f);
		if (num_col == 0) {
			for (col_pos=0; col_pos<768; col_pos++) {
				cmap[col_pos]=fli_read_char(f);
			}
			return;
		}
		for (col_cnt=skip_col; (col_cnt>0) && (col_pos<768); col_cnt--) {
			cmap[col_pos]=old_cmap[col_pos];col_pos++;
			cmap[col_pos]=old_cmap[col_pos];col_pos++;
			cmap[col_pos]=old_cmap[col_pos];col_pos++;
		}
		for (col_cnt=num_col; (col_cnt>0) && (col_pos<768); col_cnt--) {
			cmap[col_pos++]=fli_read_char(f);
			cmap[col_pos++]=fli_read_char(f);
			cmap[col_pos++]=fli_read_char(f);
		}
	}
}