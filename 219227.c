pixNumberOccupiedOctcubes(PIX       *pix,
                          l_int32    level,
                          l_int32    mincount,
                          l_float32  minfract,
                          l_int32   *pncolors)
{
l_int32    i, j, w, h, d, wpl, ncolors, size, octindex;
l_int32    rval, gval, bval;
l_int32   *carray;
l_uint32  *data, *line, *rtab, *gtab, *btab;

    PROCNAME("pixNumberOccupiedOctcubes");

    if (!pncolors)
        return ERROR_INT("&ncolors not defined", procName, 1);
    *pncolors = 0;
    if (!pix)
        return ERROR_INT("pix not defined", procName, 1);
    pixGetDimensions(pix, &w, &h, &d);
    if (d != 32)
        return ERROR_INT("pix not 32 bpp", procName, 1);
    if (level < 1 || level > 6)
        return ERROR_INT("invalid level", procName, 1);
    if ((mincount < 0 && minfract < 0) || (mincount >= 0.0 && minfract >= 0.0))
        return ERROR_INT("invalid mincount/minfract", procName, 1);
    if (mincount == 0 || minfract == 0.0)
        mincount = 1;
    else if (minfract > 0.0)
        mincount = L_MIN(1, (l_int32)(minfract * w * h));

    if (octcubeGetCount(level, &size))  /* array size = 2 ** (3 * level) */
        return ERROR_INT("size not returned", procName, 1);
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);
    if ((carray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32))) == NULL) {
        L_ERROR("carray not made\n", procName);
        goto cleanup_arrays;
    }

        /* Mark the occupied octcube leaves */
    data = pixGetData(pix);
    wpl = pixGetWpl(pix);
    for (i = 0; i < h; i++) {
        line = data + i * wpl;
        for (j = 0; j < w; j++) {
            extractRGBValues(line[j], &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            carray[octindex]++;
        }
    }

        /* Count them */
    for (i = 0, ncolors = 0; i < size; i++) {
        if (carray[i] >= mincount)
            ncolors++;
    }
    *pncolors = ncolors;

cleanup_arrays:
    LEPT_FREE(carray);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return 0;
}