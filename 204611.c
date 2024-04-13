DECLAREContigPutFunc(put8bitcmaptile)
{
    uint32** PALmap = img->PALmap;
    int samplesperpixel = img->samplesperpixel;

    (void) y;
    while (h-- > 0) {
	for (x = w; x-- > 0;)
        {
	    *cp++ = PALmap[*pp][0];
            pp += samplesperpixel;
        }
	cp += toskew;
	pp += fromskew;
    }
}