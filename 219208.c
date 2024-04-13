pixOctreeQuantByPopulation(PIX     *pixs,
                           l_int32  level,
                           l_int32  ditherflag)
{
l_int32         w, h, wpls, wpld, i, j, depth, size, ncolors, index;
l_int32         rval, gval, bval;
l_int32        *rarray, *garray, *barray, *narray, *iarray;
l_uint32        octindex, octindex2;
l_uint32       *rtab, *gtab, *btab, *rtab2, *gtab2, *btab2;
l_uint32       *lines, *lined, *datas, *datad;
L_OCTCUBE_POP  *opop;
L_HEAP         *lh;
PIX            *pixd;
PIXCMAP        *cmap;

    PROCNAME("pixOctreeQuantByPopulation");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (level == 0) level = 4;
    if (level < 3 || level > 4)
        return (PIX *)ERROR_PTR("level not in {3,4}", procName, NULL);

        /* Do not dither if image is very small */
    pixGetDimensions(pixs, &w, &h, NULL);
    if (w < MinDitherSize && h < MinDitherSize && ditherflag == 1) {
        L_INFO("Small image: dithering turned off\n", procName);
        ditherflag = 0;
    }

    if (octcubeGetCount(level, &size))  /* array size = 2 ** (3 * level) */
        return (PIX *)ERROR_PTR("size not returned", procName, NULL);
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(level, &rtab, &gtab, &btab);

    pixd = NULL;
    narray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    rarray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    garray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    barray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    if (!narray || !rarray || !garray || !barray)
        goto array_cleanup;

        /* Place the pixels in octcube leaves. */
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j++) {
            extractRGBValues(lines[j], &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            narray[octindex]++;
            rarray[octindex] += rval;
            garray[octindex] += gval;
            barray[octindex] += bval;
        }
    }

        /* Find the number of different colors */
    for (i = 0, ncolors = 0; i < size; i++) {
        if (narray[i] > 0)
            ncolors++;
    }
    if (ncolors <= 4)
        depth = 2;
    else if (ncolors <= 16)
        depth = 4;
    else
        depth = 8;
    pixd = pixCreate(w, h, depth);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    cmap = pixcmapCreate(depth);
    pixSetColormap(pixd, cmap);

        /* Average the colors in each octcube leaf. */
    for (i = 0; i < size; i++) {
        if (narray[i] > 0) {
            rarray[i] /= narray[i];
            garray[i] /= narray[i];
            barray[i] /= narray[i];
        }
    }

        /* If ncolors <= 256, finish immediately.  Do not dither.
         * Re-use narray to hold the colormap index + 1  */
    if (ncolors <= 256) {
        for (i = 0, index = 0; i < size; i++) {
            if (narray[i] > 0) {
                pixcmapAddColor(cmap, rarray[i], garray[i], barray[i]);
                narray[i] = index + 1;  /* to avoid storing 0 */
                index++;
            }
        }

            /* Set the cmap indices for each pixel */
        for (i = 0; i < h; i++) {
            lines = datas + i * wpls;
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                extractRGBValues(lines[j], &rval, &gval, &bval);
                octindex = rtab[rval] | gtab[gval] | btab[bval];
                switch (depth)
                {
                case 8:
                    SET_DATA_BYTE(lined, j, narray[octindex] - 1);
                    break;
                case 4:
                    SET_DATA_QBIT(lined, j, narray[octindex] - 1);
                    break;
                case 2:
                    SET_DATA_DIBIT(lined, j, narray[octindex] - 1);
                    break;
                default:
                    L_WARNING("shouldn't get here\n", procName);
                }
            }
        }
        goto array_cleanup;
    }

        /* More complicated.  Sort by decreasing population */
    lh = lheapCreate(500, L_SORT_DECREASING);
    for (i = 0; i < size; i++) {
        if (narray[i] > 0) {
            opop = (L_OCTCUBE_POP *)LEPT_CALLOC(1, sizeof(L_OCTCUBE_POP));
            opop->npix = (l_float32)narray[i];
            opop->index = i;
            opop->rval = rarray[i];
            opop->gval = garray[i];
            opop->bval = barray[i];
            lheapAdd(lh, opop);
        }
    }

        /* Take the top 192.  These will form the first 192 colors
         * in the cmap.  iarray[i] holds the index into the cmap. */
    iarray = (l_int32 *)LEPT_CALLOC(size, sizeof(l_int32));
    for (i = 0; i < 192; i++) {
        opop = (L_OCTCUBE_POP*)lheapRemove(lh);
        if (!opop) break;
        pixcmapAddColor(cmap, opop->rval, opop->gval, opop->bval);
        iarray[opop->index] = i + 1;  /* +1 to avoid storing 0 */

#if DEBUG_POP
        lept_stderr("i = %d, n = %6.0f, (r,g,b) = (%d %d %d)\n",
                    i, opop->npix, opop->rval, opop->gval, opop->bval);
#endif  /* DEBUG_POP */

        LEPT_FREE(opop);
    }

        /* Make the octindex tables for level 2, and reuse rarray, etc. */
    rtab2 = gtab2 = btab2 = NULL;
    makeRGBToIndexTables(2, &rtab2, &gtab2, &btab2);
    for (i = 0; i < 64; i++) {
        narray[i] = 0;
        rarray[i] = 0;
        garray[i] = 0;
        barray[i] = 0;
    }

        /* Take the rest of the occupied octcubes, assigning the pixels
         * to these new colormap indices.  iarray[] is addressed
         * by %level octcube indices, and it now holds the
         * colormap indices for all pixels in pixs.  */
    for (i = 192; i < size; i++) {
        opop = (L_OCTCUBE_POP*)lheapRemove(lh);
        if (!opop) break;
        rval = opop->rval;
        gval = opop->gval;
        bval = opop->bval;
        octindex2 = rtab2[rval] | gtab2[gval] | btab2[bval];
        narray[octindex2] += (l_int32)opop->npix;
        rarray[octindex2] += (l_int32)opop->npix * rval;
        garray[octindex2] += (l_int32)opop->npix * gval;
        barray[octindex2] += (l_int32)opop->npix * bval;
        iarray[opop->index] = 192 + octindex2 + 1;  /* +1 to avoid storing 0 */
        LEPT_FREE(opop);
    }
    lheapDestroy(&lh, TRUE);

        /* To span the full color space, which is necessary for dithering,
         * set each iarray element whose value is still 0 at the input
         * level octcube leaves (because there were no pixels in those
         * octcubes) to the colormap index corresponding to its level 2
         * octcube. */
    if (ditherflag) {
        for (i = 0; i < size; i++) {
            if (iarray[i] == 0) {
                getRGBFromOctcube(i, level, &rval, &gval, &bval);
                octindex2 = rtab2[rval] | gtab2[gval] | btab2[bval];
                iarray[i] = 192 + octindex2 + 1;
            }
        }
    }
    LEPT_FREE(rtab2);
    LEPT_FREE(gtab2);
    LEPT_FREE(btab2);

        /* Average the colors from the residuals in each level 2 octcube,
         * and add these 64 values to the colormap. */
    for (i = 0; i < 64; i++) {
        if (narray[i] > 0) {
            rarray[i] /= narray[i];
            garray[i] /= narray[i];
            barray[i] /= narray[i];
        } else {  /* no pixels in this octcube; use center value */
            getRGBFromOctcube(i, 2, &rarray[i], &garray[i], &barray[i]);
        }
        pixcmapAddColor(cmap, rarray[i], garray[i], barray[i]);
    }

        /* Set the cmap indices for each pixel.  Subtract 1 from
         * the value in iarray[] because we added 1 earlier.  */
    if (ditherflag == 0) {
        for (i = 0; i < h; i++) {
            lines = datas + i * wpls;
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                extractRGBValues(lines[j], &rval, &gval, &bval);
                octindex = rtab[rval] | gtab[gval] | btab[bval];
                SET_DATA_BYTE(lined, j, iarray[octindex] - 1);
            }
        }
    } else {  /* dither */
        pixDitherOctindexWithCmap(pixs, pixd, rtab, gtab, btab,
                                  iarray, POP_DIF_CAP);
    }

#if DEBUG_POP
    for (i = 0; i < size / 16; i++) {
        l_int32 j;
        for (j = 0; j < 16; j++)
            lept_stderr("%d ", iarray[16 * i + j]);
        lept_stderr("\n");
    }
#endif  /* DEBUG_POP */

    LEPT_FREE(iarray);

array_cleanup:
    LEPT_FREE(narray);
    LEPT_FREE(rarray);
    LEPT_FREE(garray);
    LEPT_FREE(barray);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);

    return pixd;
}