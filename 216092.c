R_API int r_str_bits64(char* strout, ut64 in) {
	int i, bit, count = 0;
	count = 0;
	for (i = (sizeof (in) * 8) - 1; i >= 0; i--) {
		bit = in >> i;
		if (bit & 1) {
			strout[count] = '1';
		} else {
			strout[count] = '0';
		}
		count++;
	}
	strout[count] = '\0';
	/* trim by 8 bits */
	trimbits (strout);
	return count;
}