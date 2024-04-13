DECLAREContigPutFunc(put4bitcmaptile)
{
    uint32** PALmap = img->PALmap;

    (void) x; (void) y;
    fromskew /= 2;
    while (h-- > 0) {
	uint32* bw;
	UNROLL2(w, bw = PALmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }
}