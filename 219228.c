pixFewColorsOctcubeQuant2(PIX      *pixs,
                          l_int32   level,
                          NUMA     *na,
                          l_int32   ncolors,
                          l_int32  *pnerrors)
{
l_int32    w, h, wpls, wpld, i, j, nerrors;
l_int32    ncubes, depth, cindex, oval;
l_int32    rval, gval, bval;
l_int32   *octarray;
l_uint32   octindex;
l_uint32  *rtab, *gtab, *btab;
l_uint32  *lines, *lined, *datas, *datad, *ppixel;
l_uint32  *colorarray;
PIX       *pixd;
PIXCMAP   *cmap;

    PROCNAME("pixFewColorsOctcubeQuant2");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (level < 3 || level > 6)
        return (PIX *)ERROR_PTR("level not in {4, 5, 6}", procName, NULL);
    if (ncolors > 256)
        return (PIX *)ERROR_PTR("ncolors > 256", procName, NULL);
    if (pnerrors)
        *pnerrors = UNDEF;

    pixd = NULL;

        /* Represent the image with a set of leaf octcubes
         * at 'level', one for each color. */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);

        /* The octarray will give a ptr from the octcube to the colorarray */
    ncubes = numaGetCount(na);
    octarray = (l_int32 *)LEPT_CALLOC(ncubes, sizeof(l_int32));

        /* The colorarray will hold the colors of the first pixel
         * that lands in the leaf octcube.  After filling, it is
         * used to generate the colormap.  */
    colorarray = (l_uint32 *)LEPT_CALLOC(ncolors + 1, sizeof(l_uint32));
    if (!octarray || !colorarray) {
        L_ERROR("octarray or colorarray not made\n", procName);
        goto cleanup_arrays;
    }

        /* Determine the output depth from the number of colors */
    pixGetDimensions(pixs, &w, &h, NULL);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    if (ncolors <= 4)
        depth = 2;
    else if (ncolors <= 16)
        depth = 4;
    else  /* ncolors <= 256 */
        depth = 8;

    if ((pixd = pixCreate(w, h, depth)) == NULL) {
        L_ERROR("pixd not made\n", procName);
        goto cleanup_arrays;
    }
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);

        /* For each pixel, get the octree index for its leaf octcube.
         * Check if a pixel has already been found in this octcube.
         *   ~ If not yet found, save that color in the colorarray
         *     and save the cindex in the octarray.
         *   ~ If already found, compare the pixel color with the
         *     color in the colorarray, and note if it differs.
         * Then set the dest pixel value to the cindex - 1, which
         * will be the cmap index for this color.  */
    cindex = 1;  /* start with 1 */
    nerrors = 0;
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            ppixel = lines + j;
            extractRGBValues(*ppixel, &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            oval = octarray[octindex];
            if (oval == 0) {
                octarray[octindex] = cindex;
                colorarray[cindex] = *ppixel;
                setPixelLow(lined, j, depth, cindex - 1);
                cindex++;
            } else {  /* already have seen this color; is it unique? */
                setPixelLow(lined, j, depth, oval - 1);
                if (colorarray[oval] != *ppixel)
                    nerrors++;
            }
        }
    }
    if (pnerrors)
        *pnerrors = nerrors;

#if  DEBUG_FEW_COLORS
    lept_stderr("ncubes = %d, ncolors = %d\n", ncubes, ncolors);
    for (i = 0; i < ncolors; i++)
        lept_stderr("color[%d] = %x\n", i, colorarray[i + 1]);
#endif  /* DEBUG_FEW_COLORS */

        /* Make the colormap. */
    cmap = pixcmapCreate(depth);
    for (i = 0; i < ncolors; i++) {
        ppixel = colorarray + i + 1;
        extractRGBValues(*ppixel, &rval, &gval, &bval);
        pixcmapAddColor(cmap, rval, gval, bval);
    }
    pixSetColormap(pixd, cmap);

cleanup_arrays:
    LEPT_FREE(octarray);
    LEPT_FREE(colorarray);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);

    return pixd;
}