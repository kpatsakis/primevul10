R_API int r_str_binstr2bin(const char *str, ut8 *out, int outlen) {
	int n, i, j, k, ret, len;
	len = strlen (str);
	for (n = i = 0; i < len; i += 8) {
		ret = 0;
		while (str[i]==' ') {
			str++;
		}
		if (i + 7 < len) {
			for (k = 0, j = i + 7; j >= i; j--, k++) {
				// INVERSE for (k=0,j=i; j<i+8; j++,k++) {
				if (str[j] == ' ') {
					//k--;
					continue;
				}
				//		printf ("---> j=%d (%c) (%02x)\n", j, str[j], str[j]);
				if (str[j] == '1') {
					ret|=1 << k;
				} else if (str[j] != '0') {
					return n;
				}
			}
		}
	//	printf ("-======> %02x\n", ret);
		out[n++] = ret;
		if (n == outlen) {
			return n;
		}
	}
	return n;
}