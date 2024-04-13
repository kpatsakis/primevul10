pixFewColorsOctcubeQuant1(PIX     *pixs,
                          l_int32  level)
{
l_int32    w, h, wpls, wpld, i, j, depth, size, ncolors, index;
l_int32    rval, gval, bval;
l_int32   *carray, *rarray, *garray, *barray;
l_uint32   octindex;
l_uint32  *rtab, *gtab, *btab;
l_uint32  *lines, *lined, *datas, *datad, *pspixel;
PIX       *pixd;
PIXCMAP   *cmap;

    PROCNAME("pixFewColorsOctcubeQuant1");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (level < 1 || level > 6)
        return (PIX *)ERROR_PTR("invalid level", procName, NULL);

    pixd = NULL;

    if (octcubeGetCount(level, &size))  /* array size = 2 ** (3 * level) */
        return (PIX *)ERROR_PTR("size not returned", procName, NULL);
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);

    carray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    rarray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    garray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    barray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    if (!carray || !rarray || !garray || !barray) {
        L_ERROR("calloc fail for an array\n", procName);
        goto array_cleanup;
    }

        /* Place the pixels in octcube leaves. */
    pixGetDimensions(pixs, &w, &h, NULL);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j++) {
            pspixel = lines + j;
            extractRGBValues(*pspixel, &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            carray[octindex]++;
            rarray[octindex] += rval;
            garray[octindex] += gval;
            barray[octindex] += bval;
        }
    }

        /* Find the number of different colors */
    for (i = 0, ncolors = 0; i < size; i++) {
        if (carray[i] > 0)
            ncolors++;
    }
    if (ncolors > 256) {
        L_WARNING("%d colors found; more than 256\n", procName, ncolors);
        goto array_cleanup;
    }
    if (ncolors <= 4)
        depth = 2;
    else if (ncolors <= 16)
        depth = 4;
    else
        depth = 8;

        /* Average the colors in each octcube leaf and add to colormap table;
         * then use carray to hold the colormap index + 1  */
    cmap = pixcmapCreate(depth);
    for (i = 0, index = 0; i < size; i++) {
        if (carray[i] > 0) {
            rarray[i] /= carray[i];
            garray[i] /= carray[i];
            barray[i] /= carray[i];
            pixcmapAddColor(cmap, rarray[i], garray[i], barray[i]);
            carray[i] = index + 1;  /* to avoid storing 0 */
            index++;
        }
    }

    pixd = pixCreate(w, h, depth);
    pixSetColormap(pixd, cmap);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            pspixel = lines + j;
            extractRGBValues(*pspixel, &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            switch (depth)
            {
            case 2:
                SET_DATA_DIBIT(lined, j, carray[octindex] - 1);
                break;
            case 4:
                SET_DATA_QBIT(lined, j, carray[octindex] - 1);
                break;
            case 8:
                SET_DATA_BYTE(lined, j, carray[octindex] - 1);
                break;
            default:
                L_WARNING("shouldn't get here\n", procName);
            }
        }
    }

array_cleanup:
    LEPT_FREE(carray);
    LEPT_FREE(rarray);
    LEPT_FREE(garray);
    LEPT_FREE(barray);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return pixd;
}