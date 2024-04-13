pixFixedOctcubeQuantGenRGB(PIX     *pixs,
                           l_int32  level)
{
l_int32    w, h, wpls, wpld, i, j;
l_int32    rval, gval, bval;
l_uint32   octindex;
l_uint32  *rtab, *gtab, *btab;
l_uint32  *lines, *lined, *datas, *datad;
PIX       *pixd;

    PROCNAME("pixFixedOctcubeQuantGenRGB");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (level < 1 || level > 6)
        return (PIX *)ERROR_PTR("level not in {1,...6}", procName, NULL);

    if (makeRGBToIndexTables(level, &rtab, &gtab, &btab))
        return (PIX *)ERROR_PTR("tables not made", procName, NULL);

    pixGetDimensions(pixs, &w, &h, NULL);
    pixd = pixCreate(w, h, 32);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            extractRGBValues(lines[j], &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            getRGBFromOctcube(octindex, level, &rval, &gval, &bval);
            composeRGBPixel(rval, gval, bval, lined + j);
        }
    }

    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return pixd;
}