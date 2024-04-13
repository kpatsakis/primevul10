DECLARESepPutFunc(putRGBUAseparate8bittile)
{
	(void) img; (void) y;
	while (h-- > 0) {
		uint32 rv, gv, bv, av;
		uint8* m;
		for (x = w; x-- > 0;) {
			av = *a++;
			m = img->UaToAa+(av<<8);
			rv = m[*r++];
			gv = m[*g++];
			bv = m[*b++];
			*cp++ = PACK4(rv,gv,bv,av);
		}
		SKEW4(r, g, b, a, fromskew);
		cp += toskew;
	}
}