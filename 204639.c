DECLARESepPutFunc(putRGBseparate16bittile)
{
	uint16 *wr = (uint16*) r;
	uint16 *wg = (uint16*) g;
	uint16 *wb = (uint16*) b;
	(void) img; (void) y; (void) a;
	while (h-- > 0) {
		for (x = 0; x < w; x++)
			*cp++ = PACK(img->Bitdepth16To8[*wr++],
			    img->Bitdepth16To8[*wg++],
			    img->Bitdepth16To8[*wb++]);
		SKEW(wr, wg, wb, fromskew);
		cp += toskew;
	}
}