DECLAREContigPutFunc(putRGBUAcontig16bittile)
{
	int samplesperpixel = img->samplesperpixel;
	uint16 *wp = (uint16 *)pp;
	(void) y;
	fromskew *= samplesperpixel;
	while (h-- > 0) {
		uint32 r,g,b,a;
		uint8* m;
		for (x = w; x-- > 0;) {
			a = img->Bitdepth16To8[wp[3]];
			m = img->UaToAa+(a<<8);
			r = m[img->Bitdepth16To8[wp[0]]];
			g = m[img->Bitdepth16To8[wp[1]]];
			b = m[img->Bitdepth16To8[wp[2]]];
			*cp++ = PACK4(r,g,b,a);
			wp += samplesperpixel;
		}
		cp += toskew;
		wp += fromskew;
	}
}