DECLAREContigPutFunc(putcontig8bitCIELab)
{
	float X, Y, Z;
	uint32 r, g, b;
	(void) y;
	fromskew *= 3;
	while (h-- > 0) {
		for (x = w; x-- > 0;) {
			TIFFCIELabToXYZ(img->cielab,
					(unsigned char)pp[0],
					(signed char)pp[1],
					(signed char)pp[2],
					&X, &Y, &Z);
			TIFFXYZToRGB(img->cielab, X, Y, Z, &r, &g, &b);
			*cp++ = PACK(r, g, b);
			pp += 3;
		}
		cp += toskew;
		pp += fromskew;
	}
}