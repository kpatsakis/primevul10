init_hex_table (void)
{
	/*
	 * We build the table at run time for several reasons:
	 *
	 * 1. portable to non-ASCII machines.
	 * 2. still reentrant since we set the init flag after setting table.
	 * 3. easier to extend.
	 * 4. less prone to bugs.
	 */
	hex_table['0'] = 0;
	hex_table['1'] = 1;
	hex_table['2'] = 2;
	hex_table['3'] = 3;
	hex_table['4'] = 4;
	hex_table['5'] = 5;
	hex_table['6'] = 6;
	hex_table['7'] = 7;
	hex_table['8'] = 8;
	hex_table['9'] = 9;
	hex_table['A'] = 10;
	hex_table['B'] = 11;
	hex_table['C'] = 12;
	hex_table['D'] = 13;
	hex_table['E'] = 14;
	hex_table['F'] = 15;
	hex_table['a'] = 10;
	hex_table['b'] = 11;
	hex_table['c'] = 12;
	hex_table['d'] = 13;
	hex_table['e'] = 14;
	hex_table['f'] = 15;

	/* delimiters of significance are flagged w/ negative value */
	hex_table[' '] = -1;
	hex_table[','] = -1;
	hex_table['}'] = -1;
	hex_table['\n'] = -1;
	hex_table['\t'] = -1;

	initialized = TRUE;
}