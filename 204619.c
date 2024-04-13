DECLAREContigPutFunc(putcontig8bitYCbCr22tile)
{
	uint32* cp2;
	int32 incr = 2*toskew+w;
	(void) y;
	fromskew = (fromskew / 2) * 6;
	cp2 = cp+w+toskew;
	while (h>=2) {
		x = w;
		while (x>=2) {
			uint32 Cb = pp[4];
			uint32 Cr = pp[5];
			YCbCrtoRGB(cp[0], pp[0]);
			YCbCrtoRGB(cp[1], pp[1]);
			YCbCrtoRGB(cp2[0], pp[2]);
			YCbCrtoRGB(cp2[1], pp[3]);
			cp += 2;
			cp2 += 2;
			pp += 6;
			x -= 2;
		}
		if (x==1) {
			uint32 Cb = pp[4];
			uint32 Cr = pp[5];
			YCbCrtoRGB(cp[0], pp[0]);
			YCbCrtoRGB(cp2[0], pp[2]);
			cp ++ ;
			cp2 ++ ;
			pp += 6;
		}
		cp += incr;
		cp2 += incr;
		pp += fromskew;
		h-=2;
	}
	if (h==1) {
		x = w;
		while (x>=2) {
			uint32 Cb = pp[4];
			uint32 Cr = pp[5];
			YCbCrtoRGB(cp[0], pp[0]);
			YCbCrtoRGB(cp[1], pp[1]);
			cp += 2;
			cp2 += 2;
			pp += 6;
			x -= 2;
		}
		if (x==1) {
			uint32 Cb = pp[4];
			uint32 Cr = pp[5];
			YCbCrtoRGB(cp[0], pp[0]);
		}
	}
}