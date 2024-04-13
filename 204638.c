DECLAREContigPutFunc(putRGBAAcontig16bittile)
{
	int samplesperpixel = img->samplesperpixel;
	uint16 *wp = (uint16 *)pp;
	(void) y;
	fromskew *= samplesperpixel;
	while (h-- > 0) {
		for (x = w; x-- > 0;) {
			*cp++ = PACK4(img->Bitdepth16To8[wp[0]],
			    img->Bitdepth16To8[wp[1]],
			    img->Bitdepth16To8[wp[2]],
			    img->Bitdepth16To8[wp[3]]);
			wp += samplesperpixel;
		}
		cp += toskew;
		wp += fromskew;
	}
}