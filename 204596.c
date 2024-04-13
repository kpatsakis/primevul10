DECLAREContigPutFunc(putcontig8bitYCbCr44tile)
{
    uint32* cp1 = cp+w+toskew;
    uint32* cp2 = cp1+w+toskew;
    uint32* cp3 = cp2+w+toskew;
    int32 incr = 3*w+4*toskew;

    (void) y;
    /* adjust fromskew */
    fromskew = (fromskew * 18) / 4;
    if ((h & 3) == 0 && (w & 3) == 0) {				        
        for (; h >= 4; h -= 4) {
            x = w>>2;
            do {
                int32 Cb = pp[16];
                int32 Cr = pp[17];

                YCbCrtoRGB(cp [0], pp[ 0]);
                YCbCrtoRGB(cp [1], pp[ 1]);
                YCbCrtoRGB(cp [2], pp[ 2]);
                YCbCrtoRGB(cp [3], pp[ 3]);
                YCbCrtoRGB(cp1[0], pp[ 4]);
                YCbCrtoRGB(cp1[1], pp[ 5]);
                YCbCrtoRGB(cp1[2], pp[ 6]);
                YCbCrtoRGB(cp1[3], pp[ 7]);
                YCbCrtoRGB(cp2[0], pp[ 8]);
                YCbCrtoRGB(cp2[1], pp[ 9]);
                YCbCrtoRGB(cp2[2], pp[10]);
                YCbCrtoRGB(cp2[3], pp[11]);
                YCbCrtoRGB(cp3[0], pp[12]);
                YCbCrtoRGB(cp3[1], pp[13]);
                YCbCrtoRGB(cp3[2], pp[14]);
                YCbCrtoRGB(cp3[3], pp[15]);

                cp += 4, cp1 += 4, cp2 += 4, cp3 += 4;
                pp += 18;
            } while (--x);
            cp += incr, cp1 += incr, cp2 += incr, cp3 += incr;
            pp += fromskew;
        }
    } else {
        while (h > 0) {
            for (x = w; x > 0;) {
                int32 Cb = pp[16];
                int32 Cr = pp[17];
                switch (x) {
                default:
                    switch (h) {
                    default: YCbCrtoRGB(cp3[3], pp[15]); /* FALLTHROUGH */
                    case 3:  YCbCrtoRGB(cp2[3], pp[11]); /* FALLTHROUGH */
                    case 2:  YCbCrtoRGB(cp1[3], pp[ 7]); /* FALLTHROUGH */
                    case 1:  YCbCrtoRGB(cp [3], pp[ 3]); /* FALLTHROUGH */
                    }                                    /* FALLTHROUGH */
                case 3:
                    switch (h) {
                    default: YCbCrtoRGB(cp3[2], pp[14]); /* FALLTHROUGH */
                    case 3:  YCbCrtoRGB(cp2[2], pp[10]); /* FALLTHROUGH */
                    case 2:  YCbCrtoRGB(cp1[2], pp[ 6]); /* FALLTHROUGH */
                    case 1:  YCbCrtoRGB(cp [2], pp[ 2]); /* FALLTHROUGH */
                    }                                    /* FALLTHROUGH */
                case 2:
                    switch (h) {
                    default: YCbCrtoRGB(cp3[1], pp[13]); /* FALLTHROUGH */
                    case 3:  YCbCrtoRGB(cp2[1], pp[ 9]); /* FALLTHROUGH */
                    case 2:  YCbCrtoRGB(cp1[1], pp[ 5]); /* FALLTHROUGH */
                    case 1:  YCbCrtoRGB(cp [1], pp[ 1]); /* FALLTHROUGH */
                    }                                    /* FALLTHROUGH */
                case 1:
                    switch (h) {
                    default: YCbCrtoRGB(cp3[0], pp[12]); /* FALLTHROUGH */
                    case 3:  YCbCrtoRGB(cp2[0], pp[ 8]); /* FALLTHROUGH */
                    case 2:  YCbCrtoRGB(cp1[0], pp[ 4]); /* FALLTHROUGH */
                    case 1:  YCbCrtoRGB(cp [0], pp[ 0]); /* FALLTHROUGH */
                    }                                    /* FALLTHROUGH */
                }
                if (x < 4) {
                    cp += x; cp1 += x; cp2 += x; cp3 += x;
                    x = 0;
                }
                else {
                    cp += 4; cp1 += 4; cp2 += 4; cp3 += 4;
                    x -= 4;
                }
                pp += 18;
            }
            if (h <= 4)
                break;
            h -= 4;
            cp += incr, cp1 += incr, cp2 += incr, cp3 += incr;
            pp += fromskew;
        }
    }
}