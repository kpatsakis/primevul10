pixOctcubeQuantFromCmap(PIX      *pixs,
                        PIXCMAP  *cmap,
                        l_int32   mindepth,
                        l_int32   level,
                        l_int32   metric)
{
l_int32   *cmaptab;
l_uint32  *rtab, *gtab, *btab;
PIX       *pixd;

    PROCNAME("pixOctcubeQuantFromCmap");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (!cmap)
        return (PIX *)ERROR_PTR("cmap not defined", procName, NULL);
    if (mindepth != 2 && mindepth != 4 && mindepth != 8)
        return (PIX *)ERROR_PTR("invalid mindepth", procName, NULL);
    if (level < 1 || level > 6)
        return (PIX *)ERROR_PTR("level not in {1...6}", procName, NULL);
    if (metric != L_MANHATTAN_DISTANCE && metric != L_EUCLIDEAN_DISTANCE)
        return (PIX *)ERROR_PTR("invalid metric", procName, NULL);

        /* Set up the tables to map rgb to the nearest colormap index */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);
    cmaptab = pixcmapToOctcubeLUT(cmap, level, metric);

    pixd = pixOctcubeQuantFromCmapLUT(pixs, cmap, mindepth,
                                      cmaptab, rtab, gtab, btab);

    LEPT_FREE(cmaptab);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return pixd;
}