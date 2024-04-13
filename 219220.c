pixcmapToOctcubeLUT(PIXCMAP  *cmap,
                    l_int32   level,
                    l_int32   metric)
{
l_int32    i, k, size, ncolors, mindist, dist, mincolor, index;
l_int32    rval, gval, bval;  /* color at center of the octcube */
l_int32   *rmap, *gmap, *bmap, *tab;

    PROCNAME("pixcmapToOctcubeLUT");

    if (!cmap)
        return (l_int32 *)ERROR_PTR("cmap not defined", procName, NULL);
    if (level < 1 || level > 6)
        return (l_int32 *)ERROR_PTR("level not in {1...6}", procName, NULL);
    if (metric != L_MANHATTAN_DISTANCE && metric != L_EUCLIDEAN_DISTANCE)
        return (l_int32 *)ERROR_PTR("invalid metric", procName, NULL);

    if (octcubeGetCount(level, &size))  /* array size = 2 ** (3 * level) */
        return (l_int32 *)ERROR_PTR("size not returned", procName, NULL);
    if ((tab = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32))) == NULL)
        return (l_int32 *)ERROR_PTR("tab not allocated", procName, NULL);

    ncolors = pixcmapGetCount(cmap);
    pixcmapToArrays(cmap, &rmap, &gmap, &bmap, NULL);

        /* Assign based on the closest octcube center to the cmap color */
    for (i = 0; i < size; i++) {
        getRGBFromOctcube(i, level, &rval, &gval, &bval);
        mindist = 1000000;
        mincolor = 0;  /* irrelevant init */
        for (k = 0; k < ncolors; k++) {
            if (metric == L_MANHATTAN_DISTANCE) {
                dist = L_ABS(rval - rmap[k]) + L_ABS(gval - gmap[k]) +
                       L_ABS(bval - bmap[k]);
            } else {  /* L_EUCLIDEAN_DISTANCE */
                dist = (rval - rmap[k]) * (rval - rmap[k]) +
                       (gval - gmap[k]) * (gval - gmap[k]) +
                       (bval - bmap[k]) * (bval - bmap[k]);
            }
            if (dist < mindist) {
                mindist = dist;
                mincolor = k;
            }
        }
        tab[i] = mincolor;
    }

        /* Reset black and white if available in the colormap.
         * The darkest octcube is at octindex 0.
         * The lightest octcube is at the max octindex. */
    pixcmapGetNearestIndex(cmap, 0, 0, 0, &index);
    pixcmapGetColor(cmap, index, &rval, &gval, &bval);
    if (rval < 7 && gval < 7 && bval < 7) {
        tab[0] = index;
    }
    pixcmapGetNearestIndex(cmap, 255, 255, 255, &index);
    pixcmapGetColor(cmap, index, &rval, &gval, &bval);
    if (rval > 248 && gval > 248 && bval > 248) {
        tab[(1 << (3 * level)) - 1] = index;
    }

    LEPT_FREE(rmap);
    LEPT_FREE(gmap);
    LEPT_FREE(bmap);
    return tab;
}