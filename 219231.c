pixOctcubeHistogram(PIX      *pixs,
                    l_int32   level,
                    l_int32  *pncolors)
{
l_int32     size, i, j, w, h, wpl, ncolors, val;
l_int32     rval, gval, bval;
l_uint32    octindex;
l_uint32   *rtab, *gtab, *btab;
l_uint32   *data, *line;
l_float32  *array;
NUMA       *na;

    PROCNAME("pixOctcubeHistogram");

    if (pncolors) *pncolors = 0;
    if (!pixs)
        return (NUMA *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (NUMA *)ERROR_PTR("pixs not 32 bpp", procName, NULL);

    pixGetDimensions(pixs, &w, &h, NULL);
    wpl = pixGetWpl(pixs);
    data = pixGetData(pixs);

    if (octcubeGetCount(level, &size))  /* array size = 2 ** (3 * level) */
        return (NUMA *)ERROR_PTR("size not returned", procName, NULL);
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);

    if ((na = numaCreate(size)) == NULL) {
        L_ERROR("na not made\n", procName);
        goto cleanup_arrays;
    }
    numaSetCount(na, size);
    array = numaGetFArray(na, L_NOCOPY);

    for (i = 0; i < h; i++) {
        line = data + i * wpl;
        for (j = 0; j < w; j++) {
            extractRGBValues(line[j], &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
#if DEBUG_OCTINDEX
            if ((level == 1 && octindex > 7) ||
                (level == 2 && octindex > 63) ||
                (level == 3 && octindex > 511) ||
                (level == 4 && octindex > 4097) ||
                (level == 5 && octindex > 32783) ||
                (level == 6 && octindex > 262271)) {
                lept_stderr("level = %d, octindex = %d, index error!\n",
                            level, octindex);
                continue;
            }
#endif  /* DEBUG_OCTINDEX */
              array[octindex] += 1.0;
        }
    }

    if (pncolors) {
        for (i = 0, ncolors = 0; i < size; i++) {
            numaGetIValue(na, i, &val);
            if (val > 0)
                ncolors++;
        }
        *pncolors = ncolors;
    }

cleanup_arrays:
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return na;
}