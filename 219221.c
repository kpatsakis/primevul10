pixOctcubeQuantMixedWithGray(PIX     *pixs,
                             l_int32  depth,
                             l_int32  graylevels,
                             l_int32  delta)
{
l_int32    w, h, wpls, wpld, i, j, size, octlevels;
l_int32    rval, gval, bval, del, val, midval;
l_int32   *carray, *rarray, *garray, *barray;
l_int32   *tabval;
l_uint32   octindex;
l_uint32  *rtab, *gtab, *btab;
l_uint32  *lines, *lined, *datas, *datad;
PIX       *pixd;
PIXCMAP   *cmap;

    PROCNAME("pixOctcubeQuantMixedWithGray");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (graylevels < 2)
        return (PIX *)ERROR_PTR("invalid graylevels", procName, NULL);
    if (depth == 4) {
        octlevels = 1;
        size = 8;   /* 2 ** 3 */
        if (graylevels > 8)
            return (PIX *)ERROR_PTR("max 8 gray levels", procName, NULL);
    } else if (depth == 8) {
        octlevels = 2;
        size = 64;   /* 2 ** 6 */
        if (graylevels > 192)
            return (PIX *)ERROR_PTR("max 192 gray levels", procName, NULL);
    } else {
        return (PIX *)ERROR_PTR("output depth not 4 or 8 bpp", procName, NULL);
    }

    pixd = NULL;

        /* Make octcube index tables */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(octlevels, &rtab, &gtab, &btab);

        /* Make octcube arrays for storing points in each cube */
    carray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    rarray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    garray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    barray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));

        /* Make lookup table, using computed thresholds  */
    tabval = makeGrayQuantIndexTable(graylevels);
    if (!rtab || !gtab || !btab ||
        !carray || !rarray || !garray || !barray || !tabval) {
        L_ERROR("calloc fail for an array\n", procName);
        goto array_cleanup;
    }

        /* Make colormapped output pixd */
    pixGetDimensions(pixs, &w, &h, NULL);
    if ((pixd = pixCreate(w, h, depth)) == NULL) {
        L_ERROR("pixd not made\n", procName);
        goto array_cleanup;
    }
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    cmap = pixcmapCreate(depth);
    for (j = 0; j < size; j++)  /* reserve octcube colors */
        pixcmapAddColor(cmap, 1, 1, 1);  /* a color that won't be used */
    for (j = 0; j < graylevels; j++) {  /* set grayscale colors */
        val = (255 * j) / (graylevels - 1);
        pixcmapAddColor(cmap, val, val, val);
    }
    pixSetColormap(pixd, cmap);
    wpld = pixGetWpl(pixd);
    datad = pixGetData(pixd);

        /* Go through src image: assign dest pixels to colormap values
         * and compute average colors in each occupied octcube */
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            extractRGBValues(lines[j], &rval, &gval, &bval);
            if (rval > gval) {
                if (gval > bval) {   /* r > g > b */
                    del = rval - bval;
                    midval = gval;
                } else if (rval > bval) {  /* r > b > g */
                    del = rval - gval;
                    midval = bval;
                } else {  /* b > r > g */
                    del = bval - gval;
                    midval = rval;
                }
            } else {  /* gval >= rval */
                if (rval > bval) {  /* g > r > b */
                    del = gval - bval;
                    midval = rval;
                } else if (gval > bval) {  /* g > b > r */
                    del = gval - rval;
                    midval = bval;
                } else {  /* b > g > r */
                    del = bval - rval;
                    midval = gval;
                }
            }
            if (del > delta) {  /* assign to color */
                octindex = rtab[rval] | gtab[gval] | btab[bval];
                carray[octindex]++;
                rarray[octindex] += rval;
                garray[octindex] += gval;
                barray[octindex] += bval;
                if (depth == 4)
                    SET_DATA_QBIT(lined, j, octindex);
                else  /* depth == 8 */
                    SET_DATA_BYTE(lined, j, octindex);
            } else {  /* assign to grayscale */
                val = size + tabval[midval];
                if (depth == 4)
                    SET_DATA_QBIT(lined, j, val);
                else  /* depth == 8 */
                    SET_DATA_BYTE(lined, j, val);
            }
        }
    }

        /* Average the colors in each bin and reset the colormap */
    for (i = 0; i < size; i++) {
        if (carray[i] > 0) {
            rarray[i] /= carray[i];
            garray[i] /= carray[i];
            barray[i] /= carray[i];
            pixcmapResetColor(cmap, i, rarray[i], garray[i], barray[i]);
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
    LEPT_FREE(tabval);

    return pixd;
}