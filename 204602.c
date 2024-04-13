DECLARESepPutFunc(putRGBAAseparate16bittile)
{
	uint16 *wr = (uint16*) r;
	uint16 *wg = (uint16*) g;
	uint16 *wb = (uint16*) b;
	uint16 *wa = (uint16*) a;
	(void) img; (void) y;
	while (h-- > 0) {
		for (x = 0; x < w; x++)
			*cp++ = PACK4(img->Bitdepth16To8[*wr++],
			    img->Bitdepth16To8[*wg++],
			    img->Bitdepth16To8[*wb++],
			    img->Bitdepth16To8[*wa++]);
		SKEW4(wr, wg, wb, wa, fromskew);
		cp += toskew;
	}
}