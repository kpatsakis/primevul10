DECLAREContigPutFunc(putcontig8bitYCbCr12tile)
{
	uint32* cp2;
	int32 incr = 2*toskew+w;
	(void) y;
	fromskew = (fromskew / 2) * 4;
	cp2 = cp+w+toskew;
	while (h>=2) {
		x = w;
		do {
			uint32 Cb = pp[2];
			uint32 Cr = pp[3];
			YCbCrtoRGB(cp[0], pp[0]);
			YCbCrtoRGB(cp2[0], pp[1]);
			cp ++;
			cp2 ++;
			pp += 4;
		} while (--x);
		cp += incr;
		cp2 += incr;
		pp += fromskew;
		h-=2;
	}
	if (h==1) {
		x = w;
		do {
			uint32 Cb = pp[2];
			uint32 Cr = pp[3];
			YCbCrtoRGB(cp[0], pp[0]);
			cp ++;
			pp += 4;
		} while (--x);
	}
}