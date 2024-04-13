DECLAREContigPutFunc(putRGBUAcontig8bittile)
{
	int samplesperpixel = img->samplesperpixel;
	(void) y;
	fromskew *= samplesperpixel;
	while (h-- > 0) {
		uint32 r, g, b, a;
		uint8* m;
		for (x = w; x-- > 0;) {
			a = pp[3];
			m = img->UaToAa+((size_t) a<<8);
			r = m[pp[0]];
			g = m[pp[1]];
			b = m[pp[2]];
			*cp++ = PACK4(r,g,b,a);
			pp += samplesperpixel;
		}
		cp += toskew;
		pp += fromskew;
	}
}