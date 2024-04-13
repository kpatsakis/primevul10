pixRemoveUnusedColors(PIX  *pixs)
{
l_int32     i, j, w, h, d, nc, wpls, val, newval, index, zerofound;
l_int32     rval, gval, bval;
l_uint32   *datas, *lines;
l_int32    *histo, *map1, *map2;
PIXCMAP    *cmap, *cmapd;

    PROCNAME("pixRemoveUnusedColors");

    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if ((cmap = pixGetColormap(pixs)) == NULL)
        return 0;

    d = pixGetDepth(pixs);
    if (d != 2 && d != 4 && d != 8)
        return ERROR_INT("d not in {2, 4, 8}", procName, 1);

        /* Find which indices are actually used */
    nc = pixcmapGetCount(cmap);
    if ((histo = (l_int32 *)LEPT_CALLOC(nc, sizeof(l_int32))) == NULL)
        return ERROR_INT("histo not made", procName, 1);
    pixGetDimensions(pixs, &w, &h, NULL);
    wpls = pixGetWpl(pixs);
    datas = pixGetData(pixs);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j++) {
            switch (d)
            {
            case 2:
                val = GET_DATA_DIBIT(lines, j);
                break;
            case 4:
                val = GET_DATA_QBIT(lines, j);
                break;
            case 8:
                val = GET_DATA_BYTE(lines, j);
                break;
            default:
                LEPT_FREE(histo);
                return ERROR_INT("switch ran off end!", procName, 1);
            }
            if (val >= nc) {
                L_WARNING("cmap index out of bounds!\n", procName);
                continue;
            }
            histo[val]++;
        }
    }

        /* Check if there are any zeroes.  If none, quit. */
    zerofound = FALSE;
    for (i = 0; i < nc; i++) {
        if (histo[i] == 0) {
            zerofound = TRUE;
            break;
        }
    }
    if (!zerofound) {
      LEPT_FREE(histo);
      return 0;
    }

        /* Generate mapping tables between indices */
    map1 = (l_int32 *)LEPT_CALLOC(nc, sizeof(l_int32));
    map2 = (l_int32 *)LEPT_CALLOC(nc, sizeof(l_int32));
    index = 0;
    for (i = 0; i < nc; i++) {
        if (histo[i] != 0) {
            map1[index] = i;  /* get old index from new */
            map2[i] = index;  /* get new index from old */
            index++;
        }
    }

        /* Generate new colormap and attach to pixs */
    cmapd = pixcmapCreate(d);
    for (i = 0; i < index; i++) {
        pixcmapGetColor(cmap, map1[i], &rval, &gval, &bval);
        pixcmapAddColor(cmapd, rval, gval, bval);
    }
    pixSetColormap(pixs, cmapd);

        /* Map pixel (index) values to new cmap */
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j++) {
            switch (d)
            {
            case 2:
                val = GET_DATA_DIBIT(lines, j);
                newval = map2[val];
                SET_DATA_DIBIT(lines, j, newval);
                break;
            case 4:
                val = GET_DATA_QBIT(lines, j);
                newval = map2[val];
                SET_DATA_QBIT(lines, j, newval);
                break;
            case 8:
                val = GET_DATA_BYTE(lines, j);
                newval = map2[val];
                SET_DATA_BYTE(lines, j, newval);
                break;
            default:
                LEPT_FREE(histo);
                LEPT_FREE(map1);
                LEPT_FREE(map2);
                return ERROR_INT("switch ran off end!", procName, 1);
            }
        }
    }

    LEPT_FREE(histo);
    LEPT_FREE(map1);
    LEPT_FREE(map2);
    return 0;
}