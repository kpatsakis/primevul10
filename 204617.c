DECLARESepPutFunc(putCMYKseparate8bittile)
{
	(void) img; (void) y;
	while (h-- > 0) {
		uint32 rv, gv, bv, kv;
		for (x = w; x-- > 0;) {
			kv = 255 - *a++;
			rv = (kv*(255-*r++))/255;
			gv = (kv*(255-*g++))/255;
			bv = (kv*(255-*b++))/255;
			*cp++ = PACK4(rv,gv,bv,255);
		}
		SKEW4(r, g, b, a, fromskew);
		cp += toskew;
	}
}