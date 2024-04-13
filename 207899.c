probably_utf8_chunk(pTHX_ char *s, STRLEN len)
{
    char *e = s + len;
    STRLEN clen;

    /* ignore partial utf8 char at end of buffer */
    while (s < e && UTF8_IS_CONTINUATION((U8)*(e - 1)))
	e--;
    if (s < e && UTF8_IS_START((U8)*(e - 1)))
	e--;
    clen = len - (e - s);
    if (clen && UTF8SKIP(e) == clen) {
	/* all promised continuation bytes are present */
	e = s + len;
    }

    if (!has_hibit(s, e))
	return 0;

    return is_utf8_string((U8*)s, e - s);
}