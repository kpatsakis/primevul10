pixQuantFromCmap(PIX      *pixs,
                 PIXCMAP  *cmap,
                 l_int32   mindepth,
                 l_int32   level,
                 l_int32   metric)
{
l_int32  d;

    PROCNAME("pixQuantFromCmap");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (mindepth != 2 && mindepth != 4 && mindepth != 8)
        return (PIX *)ERROR_PTR("invalid mindepth", procName, NULL);
    d = pixGetDepth(pixs);
    if (d == 8)
        return pixGrayQuantFromCmap(pixs, cmap, mindepth);
    else if (d == 32)
        return pixOctcubeQuantFromCmap(pixs, cmap, mindepth,
                                       level, metric);
    else
        return (PIX *)ERROR_PTR("d not 8 or 32 bpp", procName, NULL);
}