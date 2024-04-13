DECLARESepPutFunc(putRGBUAseparate16bittile)
{
	uint16 *wr = (uint16*) r;
	uint16 *wg = (uint16*) g;
	uint16 *wb = (uint16*) b;
	uint16 *wa = (uint16*) a;
	(void) img; (void) y;
	while (h-- > 0) {
		uint32 r2,g2,b2,a2;
		uint8* m;
		for (x = w; x-- > 0;) {
			a2 = img->Bitdepth16To8[*wa++];
			m = img->UaToAa+((size_t) a2<<8);
			r2 = m[img->Bitdepth16To8[*wr++]];
			g2 = m[img->Bitdepth16To8[*wg++]];
			b2 = m[img->Bitdepth16To8[*wb++]];
			*cp++ = PACK4(r2,g2,b2,a2);
		}
		SKEW4(wr, wg, wb, wa, fromskew);
		cp += toskew;
	}
}