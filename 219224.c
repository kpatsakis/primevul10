pixOctreeColorQuant(PIX     *pixs,
                    l_int32  colors,
                    l_int32  ditherflag)
{
    PROCNAME("pixOctreeColorQuant");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (colors < 128 || colors > 240)  /* further restricted */
        return (PIX *)ERROR_PTR("colors must be in [128, 240]", procName, NULL);

    return pixOctreeColorQuantGeneral(pixs, colors, ditherflag, 0.01, 0.01);
}