pixOctreeQuantNumColors(PIX     *pixs,
                        l_int32  maxcolors,
                        l_int32  subsample)
{
l_int32    w, h, minside, bpp, wpls, wpld, i, j, actualcolors;
l_int32    rval, gval, bval, nbase, nextra, maxlevel, ncubes, val;
l_int32   *lut1, *lut2;
l_uint32   index;
l_uint32  *lines, *lined, *datas, *datad, *pspixel;
l_uint32  *rtab, *gtab, *btab;
OQCELL    *oqc;
OQCELL   **oqca;
L_HEAP    *lh;
PIX       *pixd;
PIXCMAP   *cmap;

    PROCNAME("pixOctreeQuantNumColors");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (maxcolors < 8) {
        L_WARNING("max colors < 8; setting to 8\n", procName);
        maxcolors = 8;
    }
    if (maxcolors > 256) {
        L_WARNING("max colors > 256; setting to 256\n", procName);
        maxcolors = 256;
    }

    pixGetDimensions(pixs, &w, &h, NULL);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    minside = L_MIN(w, h);
    if (subsample <= 0) {
       subsample = L_MAX(1, minside / 200);
    }

    if (maxcolors <= 16) {
        bpp = 4;
        pixd = pixCreate(w, h, bpp);
        maxlevel = 2;
        ncubes = 64;   /* 2^6 */
        nbase = 8;
        nextra = maxcolors - nbase;
    } else if (maxcolors <= 64) {
        bpp = 8;
        pixd = pixCreate(w, h, bpp);
        maxlevel = 2;
        ncubes = 64;  /* 2^6 */
        nbase = 8;
        nextra = maxcolors - nbase;
    } else {  /* maxcolors <= 256 */
        bpp = 8;
        pixd = pixCreate(w, h, bpp);
        maxlevel = 3;
        ncubes = 512;  /* 2^9 */
        nbase = 64;
        nextra = maxcolors - nbase;
    }

    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);

        /*----------------------------------------------------------*
         * If we're using the minimum number of colors, it is       *
         * much simpler.  We just use 'nbase' octcubes.             *
         * For this case, we don't eliminate any extra colors.      *
         *----------------------------------------------------------*/
    if (nextra == 0) {
            /* prepare the OctcubeQuantCell array */
        if ((oqca = (OQCELL **)LEPT_CALLOC(nbase, sizeof(OQCELL *))) == NULL) {
            pixDestroy(&pixd);
            return (PIX *)ERROR_PTR("oqca not made", procName, NULL);
        }
        for (i = 0; i < nbase; i++) {
            oqca[i] = (OQCELL *)LEPT_CALLOC(1, sizeof(OQCELL));
            oqca[i]->n = 0.0;
        }

        rtab = gtab = btab = NULL;
        makeRGBToIndexTables(maxlevel - 1, &rtab, &gtab, &btab);

            /* Go through the entire image, gathering statistics and
             * assigning pixels to their quantized value */
        datad = pixGetData(pixd);
        wpld = pixGetWpl(pixd);
        for (i = 0; i < h; i++) {
            lines = datas + i * wpls;
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                pspixel = lines + j;
                extractRGBValues(*pspixel, &rval, &gval, &bval);
                getOctcubeIndexFromRGB(rval, gval, bval,
                                       rtab, gtab, btab, &index);
/*                lept_stderr("rval = %d, gval = %d, bval = %d,"
                              " index = %d\n", rval, gval, bval, index); */
                if (bpp == 4)
                    SET_DATA_QBIT(lined, j, index);
                else  /* bpp == 8 */
                    SET_DATA_BYTE(lined, j, index);
                oqca[index]->n += 1.0;
                oqca[index]->rcum += rval;
                oqca[index]->gcum += gval;
                oqca[index]->bcum += bval;
            }
        }

            /* Compute average color values in each octcube, and
             * generate colormap */
        cmap = pixcmapCreate(bpp);
        pixSetColormap(pixd, cmap);
        for (i = 0; i < nbase; i++) {
            oqc = oqca[i];
            if (oqc->n != 0) {
                oqc->rval = (l_int32)(oqc->rcum / oqc->n);
                oqc->gval = (l_int32)(oqc->gcum / oqc->n);
                oqc->bval = (l_int32)(oqc->bcum / oqc->n);
            } else {
                getRGBFromOctcube(i, maxlevel - 1, &oqc->rval,
                                  &oqc->gval, &oqc->bval);
            }
            pixcmapAddColor(cmap, oqc->rval, oqc->gval, oqc->bval);
        }

        for (i = 0; i < nbase; i++)
            LEPT_FREE(oqca[i]);
        LEPT_FREE(oqca);
        LEPT_FREE(rtab);
        LEPT_FREE(gtab);
        LEPT_FREE(btab);
        return pixd;
    }

        /*------------------------------------------------------------*
         * General case: we will use colors in octcubes at maxlevel.  *
         * We also remove any colors that are not populated from      *
         * the colormap.                                              *
         *------------------------------------------------------------*/
        /* Prepare the OctcubeQuantCell array */
    if ((oqca = (OQCELL **)LEPT_CALLOC(ncubes, sizeof(OQCELL *))) == NULL) {
        pixDestroy(&pixd);
        return (PIX *)ERROR_PTR("oqca not made", procName, NULL);
    }
    for (i = 0; i < ncubes; i++) {
        oqca[i] = (OQCELL *)LEPT_CALLOC(1, sizeof(OQCELL));
        oqca[i]->n = 0.0;
    }

        /* Make the tables to map color to the octindex,
         * of which there are 'ncubes' at 'maxlevel' */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(maxlevel, &rtab, &gtab, &btab);

        /* Estimate the color distribution; we want to find the
         * most popular nextra colors at 'maxlevel' */
    for (i = 0; i < h; i += subsample) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j += subsample) {
            pspixel = lines + j;
            extractRGBValues(*pspixel, &rval, &gval, &bval);
            getOctcubeIndexFromRGB(rval, gval, bval, rtab, gtab, btab, &index);
            oqca[index]->n += 1.0;
            oqca[index]->octindex = index;
            oqca[index]->rcum += rval;
            oqca[index]->gcum += gval;
            oqca[index]->bcum += bval;
        }
    }

        /* Transfer the OQCELL from the array, and order in a heap */
    lh = lheapCreate(512, L_SORT_DECREASING);
    for (i = 0; i < ncubes; i++)
        lheapAdd(lh, oqca[i]);
    LEPT_FREE(oqca);  /* don't need this array */

        /* Prepare a new OctcubeQuantCell array, with maxcolors cells  */
    oqca = (OQCELL **)LEPT_CALLOC(maxcolors, sizeof(OQCELL *));
    for (i = 0; i < nbase; i++) {  /* make nbase cells */
        oqca[i] = (OQCELL *)LEPT_CALLOC(1, sizeof(OQCELL));
        oqca[i]->n = 0.0;
    }

        /* Remove the nextra most populated ones, and put them in the array */
    for (i = 0; i < nextra; i++) {
        oqc = (OQCELL *)lheapRemove(lh);
        oqc->n = 0.0;  /* reinit */
        oqc->rcum = 0;
        oqc->gcum = 0;
        oqc->bcum = 0;
        oqca[nbase + i] = oqc;  /* store it in the array */
    }

        /* Destroy the heap and its remaining contents */
    lheapDestroy(&lh, TRUE);

        /* Generate a lookup table from octindex at maxlevel
         * to color table index */
    lut1 = (l_int32 *)LEPT_CALLOC(ncubes, sizeof(l_int32));
    for (i = 0; i < nextra; i++)
        lut1[oqca[nbase + i]->octindex] = nbase + i;
    for (index = 0; index < ncubes; index++) {
        if (lut1[index] == 0)  /* not one of the extras; need to assign */
            lut1[index] = index >> 3;  /* remove the least significant bits */
/*        lept_stderr("lut1[%d] = %d\n", index, lut1[index]); */
    }

        /* Go through the entire image, gathering statistics and
         * assigning pixels to their quantized value */
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            pspixel = lines + j;
            extractRGBValues(*pspixel, &rval, &gval, &bval);
            getOctcubeIndexFromRGB(rval, gval, bval, rtab, gtab, btab, &index);
/*            lept_stderr("rval = %d, gval = %d, bval = %d, index = %d\n",
                          rval, gval, bval, index); */
            val = lut1[index];
            switch (bpp) {
            case 4:
                SET_DATA_QBIT(lined, j, val);
                break;
            case 8:
                SET_DATA_BYTE(lined, j, val);
                break;
            default:
                LEPT_FREE(oqca);
                LEPT_FREE(lut1);
                return (PIX *)ERROR_PTR("bpp not 4 or 8!", procName, NULL);
                break;
            }
            oqca[val]->n += 1.0;
            oqca[val]->rcum += rval;
            oqca[val]->gcum += gval;
            oqca[val]->bcum += bval;
        }
    }

        /* Compute averages, set up a colormap, and make a second
         * lut that converts from the color values currently in
         * the image to a minimal set */
    lut2 = (l_int32 *)LEPT_CALLOC(ncubes, sizeof(l_int32));
    cmap = pixcmapCreate(bpp);
    pixSetColormap(pixd, cmap);
    for (i = 0, index = 0; i < maxcolors; i++) {
        oqc = oqca[i];
        lut2[i] = index;
        if (oqc->n == 0)  /* no occupancy; don't bump up index */
            continue;
        oqc->rval = (l_int32)(oqc->rcum / oqc->n);
        oqc->gval = (l_int32)(oqc->gcum / oqc->n);
        oqc->bval = (l_int32)(oqc->bcum / oqc->n);
        pixcmapAddColor(cmap, oqc->rval, oqc->gval, oqc->bval);
        index++;
    }
/*    pixcmapWriteStream(stderr, cmap); */
    actualcolors = pixcmapGetCount(cmap);
/*    lept_stderr("Number of different colors = %d\n", actualcolors); */

        /* Last time through the image; use the lookup table to
         * remap the pixel value to the minimal colormap */
    if (actualcolors < maxcolors) {
        for (i = 0; i < h; i++) {
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                switch (bpp) {
                case 4:
                    val = GET_DATA_QBIT(lined, j);
                    SET_DATA_QBIT(lined, j, lut2[val]);
                    break;
                case 8:
                    val = GET_DATA_BYTE(lined, j);
                    SET_DATA_BYTE(lined, j, lut2[val]);
                    break;
                }
            }
        }
    }

    if (oqca) {
        for (i = 0; i < maxcolors; i++)
            LEPT_FREE(oqca[i]);
    }
    LEPT_FREE(oqca);
    LEPT_FREE(lut1);
    LEPT_FREE(lut2);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return pixd;
}