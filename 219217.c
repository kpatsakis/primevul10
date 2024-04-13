pixOctcubeQuantFromCmapLUT(PIX       *pixs,
                           PIXCMAP   *cmap,
                           l_int32    mindepth,
                           l_int32   *cmaptab,
                           l_uint32  *rtab,
                           l_uint32  *gtab,
                           l_uint32  *btab)
{
l_int32    i, j, w, h, depth, wpls, wpld;
l_int32    rval, gval, bval, index;
l_uint32   octindex;
l_uint32  *lines, *lined, *datas, *datad;
PIX       *pixd;
PIXCMAP   *cmapc;

    PROCNAME("pixOctcubeQuantFromCmapLUT");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (!cmap)
        return (PIX *)ERROR_PTR("cmap not defined", procName, NULL);
    if (mindepth != 2 && mindepth != 4 && mindepth != 8)
        return (PIX *)ERROR_PTR("invalid mindepth", procName, NULL);
    if (!rtab || !gtab || !btab || !cmaptab)
        return (PIX *)ERROR_PTR("tables not all defined", procName, NULL);

        /* Init dest pix (with minimum bpp depending on cmap) */
    pixcmapGetMinDepth(cmap, &depth);
    depth = L_MAX(depth, mindepth);
    pixGetDimensions(pixs, &w, &h, NULL);
    if ((pixd = pixCreate(w, h, depth)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    cmapc = pixcmapCopy(cmap);
    pixSetColormap(pixd, cmapc);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);

        /* Insert the colormap index of the color nearest to the input pixel */
    datas = pixGetData(pixs);
    datad = pixGetData(pixd);
    wpls = pixGetWpl(pixs);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            extractRGBValues(lines[j], &rval, &gval, &bval);
                /* Map from rgb to octcube index */
            getOctcubeIndexFromRGB(rval, gval, bval, rtab, gtab, btab,
                                   &octindex);
                /* Map from octcube index to nearest colormap index */
            index = cmaptab[octindex];
            if (depth == 2)
                SET_DATA_DIBIT(lined, j, index);
            else if (depth == 4)
                SET_DATA_QBIT(lined, j, index);
            else  /* depth == 8 */
                SET_DATA_BYTE(lined, j, index);
        }
    }

    return pixd;
}