DECLARESepPutFunc(putseparate8bitYCbCr11tile)
{
	(void) y;
	(void) a;
	/* TODO: naming of input vars is still off, change obfuscating declaration inside define, or resolve obfuscation */
	while (h-- > 0) {
		x = w;
		do {
			uint32 dr, dg, db;
			TIFFYCbCrtoRGB(img->ycbcr,*r++,*g++,*b++,&dr,&dg,&db);
			*cp++ = PACK(dr,dg,db);
		} while (--x);
		SKEW(r, g, b, fromskew);
		cp += toskew;
	}
}