DECLAREContigPutFunc(putRGBAAcontig8bittile)
{
    int samplesperpixel = img->samplesperpixel;

    (void) x; (void) y;
    fromskew *= samplesperpixel;
    while (h-- > 0) {
	UNROLL8(w, NOP,
	    *cp++ = PACK4(pp[0], pp[1], pp[2], pp[3]);
	    pp += samplesperpixel);
	cp += toskew;
	pp += fromskew;
    }
}