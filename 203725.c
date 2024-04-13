malformed (void)
{
    char numbuf[LINENUM_LENGTH_BOUND + 1];
    fatal ("malformed patch at line %s: %s",
	   format_linenum (numbuf, p_input_line), buf);
		/* about as informative as "Syntax error" in C */
}