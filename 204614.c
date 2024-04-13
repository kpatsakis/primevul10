DECLAREContigPutFunc(putRGBcontig8bitCMYKtile)
{
    int samplesperpixel = img->samplesperpixel;
    uint16 r, g, b, k;

    (void) x; (void) y;
    fromskew *= samplesperpixel;
    while (h-- > 0) {
	UNROLL8(w, NOP,
	    k = 255 - pp[3];
	    r = (k*(255-pp[0]))/255;
	    g = (k*(255-pp[1]))/255;
	    b = (k*(255-pp[2]))/255;
	    *cp++ = PACK(r, g, b);
	    pp += samplesperpixel);
	cp += toskew;
	pp += fromskew;
    }
}