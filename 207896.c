has_hibit(char *s, char *e)
{
    while (s < e) {
	U8 ch = *s++;
	if (!UTF8_IS_INVARIANT(ch)) {
	    return 1;
	}
    }
    return 0;
}