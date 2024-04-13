DECLARESepPutFunc(putRGBAAseparate8bittile)
{
	(void) img; (void) x; (void) y; 
	while (h-- > 0) {
		UNROLL8(w, NOP, *cp++ = PACK4(*r++, *g++, *b++, *a++));
		SKEW4(r, g, b, a, fromskew);
		cp += toskew;
	}
}