next_int (FILE *fstream)
{
	int ch;
	int value = 0;
	int gotone = 0;
	int done = 0;
    
	/* loop, accumulate hex value until find delimiter 
	   skip any initial delimiters found in read stream */

	while (!done) {
		ch = getc (fstream);
		if (ch == EOF) {
			value = -1;
			done++;
		} else {
			/* trim high bits, check type and accumulate */
			ch &= 0xff;
			if (g_ascii_isxdigit (ch)) {
				value = (value << 4) + g_ascii_xdigit_value (ch);
				gotone++;
			} else if ((hex_table[ch]) < 0 && gotone) {
				done++;
			}
		}
	}
	return value;
}