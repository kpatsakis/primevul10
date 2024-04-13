DECLAREContigPutFunc(putgreytile)
{
    int samplesperpixel = img->samplesperpixel;
    uint32** BWmap = img->BWmap;

    (void) y;
    while (h-- > 0) {
	for (x = w; x-- > 0;)
        {
	    *cp++ = BWmap[*pp][0];
            pp += samplesperpixel;
        }
	cp += toskew;
	pp += fromskew;
    }
}