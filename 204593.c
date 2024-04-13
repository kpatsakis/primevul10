DECLAREContigPutFunc(put2bitcmaptile)
{
    uint32** PALmap = img->PALmap;

    (void) x; (void) y;
    fromskew /= 4;
    while (h-- > 0) {
	uint32* bw;
	UNROLL4(w, bw = PALmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }
}