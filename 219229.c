pixOctreeQuantizePixels(PIX       *pixs,
                        CQCELL  ***cqcaa,
                        l_int32    ditherflag)
{
l_uint8   *bufu8r, *bufu8g, *bufu8b;
l_int32    rval, gval, bval;
l_int32    octindex, index;
l_int32    val1, val2, val3, dif;
l_int32    w, h, wpls, wpld, i, j, success;
l_int32    rc, gc, bc;
l_int32   *buf1r, *buf1g, *buf1b, *buf2r, *buf2g, *buf2b;
l_uint32  *rtab, *gtab, *btab;
l_uint32  *datas, *datad, *lines, *lined;
PIX       *pixd;

    PROCNAME("pixOctreeQuantizePixels");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs must be 32 bpp", procName, NULL);
    if (!cqcaa)
        return (PIX *)ERROR_PTR("cqcaa not defined", procName, NULL);

        /* Make output 8 bpp palette image */
    pixGetDimensions(pixs, &w, &h, NULL);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    if ((pixd = pixCreate(w, h, 8)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);

        /* Make the canonical index tables */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(CqNLevels, &rtab, &gtab, &btab);

        /* Traverse tree from root, looking for lowest cube
         * that is a leaf, and set dest pix to its
         * colortable index value.  The results are far
         * better when dithering to get a more accurate
         * average color.  */
    if (ditherflag == 0) {    /* no dithering */
        for (i = 0; i < h; i++) {
            lines = datas + i * wpls;
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                extractRGBValues(lines[j], &rval, &gval, &bval);
                octindex = rtab[rval] | gtab[gval] | btab[bval];
                octreeFindColorCell(octindex, cqcaa, &index, &rc, &gc, &bc);
                SET_DATA_BYTE(lined, j, index);
            }
        }
    } else {  /* Dither */
        success = TRUE;
        bufu8r = bufu8g = bufu8b = NULL;
        buf1r = buf1g = buf1b = buf2r = buf2g = buf2b = NULL;
        bufu8r = (l_uint8 *)LEPT_CALLOC(w, sizeof(l_uint8));
        bufu8g = (l_uint8 *)LEPT_CALLOC(w, sizeof(l_uint8));
        bufu8b = (l_uint8 *)LEPT_CALLOC(w, sizeof(l_uint8));
        buf1r = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        buf1g = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        buf1b = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        buf2r = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        buf2g = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        buf2b = (l_int32 *)LEPT_CALLOC(w, sizeof(l_int32));
        if (!bufu8r || !bufu8g || !bufu8b || !buf1r || !buf1g ||
            !buf1b || !buf2r || !buf2g || !buf2b) {
            L_ERROR("buffer not made\n", procName);
            success = FALSE;
            goto buffer_cleanup;
        }

            /* Start by priming buf2; line 1 is above line 2 */
        pixGetRGBLine(pixs, 0, bufu8r, bufu8g, bufu8b);
        for (j = 0; j < w; j++) {
            buf2r[j] = 64 * bufu8r[j];
            buf2g[j] = 64 * bufu8g[j];
            buf2b[j] = 64 * bufu8b[j];
        }

        for (i = 0; i < h - 1; i++) {
                /* Swap data 2 --> 1, and read in new line 2 */
            memcpy(buf1r, buf2r, 4 * w);
            memcpy(buf1g, buf2g, 4 * w);
            memcpy(buf1b, buf2b, 4 * w);
            pixGetRGBLine(pixs, i + 1, bufu8r, bufu8g, bufu8b);
            for (j = 0; j < w; j++) {
                buf2r[j] = 64 * bufu8r[j];
                buf2g[j] = 64 * bufu8g[j];
                buf2b[j] = 64 * bufu8b[j];
            }

                /* Dither */
            lined = datad + i * wpld;
            for (j = 0; j < w - 1; j++) {
                rval = buf1r[j] / 64;
                gval = buf1g[j] / 64;
                bval = buf1b[j] / 64;
                octindex = rtab[rval] | gtab[gval] | btab[bval];
                octreeFindColorCell(octindex, cqcaa, &index, &rc, &gc, &bc);
                SET_DATA_BYTE(lined, j, index);

                dif = buf1r[j] / 8 - 8 * rc;
                if (dif != 0) {
                    val1 = buf1r[j + 1] + 3 * dif;
                    val2 = buf2r[j] + 3 * dif;
                    val3 = buf2r[j + 1] + 2 * dif;
                    if (dif > 0) {
                        buf1r[j + 1] = L_MIN(16383, val1);
                        buf2r[j] = L_MIN(16383, val2);
                        buf2r[j + 1] = L_MIN(16383, val3);
                    } else {
                        buf1r[j + 1] = L_MAX(0, val1);
                        buf2r[j] = L_MAX(0, val2);
                        buf2r[j + 1] = L_MAX(0, val3);
                    }
                }

                dif = buf1g[j] / 8 - 8 * gc;
                if (dif != 0) {
                    val1 = buf1g[j + 1] + 3 * dif;
                    val2 = buf2g[j] + 3 * dif;
                    val3 = buf2g[j + 1] + 2 * dif;
                    if (dif > 0) {
                        buf1g[j + 1] = L_MIN(16383, val1);
                        buf2g[j] = L_MIN(16383, val2);
                        buf2g[j + 1] = L_MIN(16383, val3);
                    } else {
                        buf1g[j + 1] = L_MAX(0, val1);
                        buf2g[j] = L_MAX(0, val2);
                        buf2g[j + 1] = L_MAX(0, val3);
                    }
                }

                dif = buf1b[j] / 8 - 8 * bc;
                if (dif != 0) {
                    val1 = buf1b[j + 1] + 3 * dif;
                    val2 = buf2b[j] + 3 * dif;
                    val3 = buf2b[j + 1] + 2 * dif;
                    if (dif > 0) {
                        buf1b[j + 1] = L_MIN(16383, val1);
                        buf2b[j] = L_MIN(16383, val2);
                        buf2b[j + 1] = L_MIN(16383, val3);
                    } else {
                        buf1b[j + 1] = L_MAX(0, val1);
                        buf2b[j] = L_MAX(0, val2);
                        buf2b[j + 1] = L_MAX(0, val3);
                    }
                }
            }

                /* Get last pixel in row; no downward propagation */
            rval = buf1r[w - 1] / 64;
            gval = buf1g[w - 1] / 64;
            bval = buf1b[w - 1] / 64;
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            octreeFindColorCell(octindex, cqcaa, &index, &rc, &gc, &bc);
            SET_DATA_BYTE(lined, w - 1, index);
        }

            /* Get last row of pixels; no leftward propagation */
        lined = datad + (h - 1) * wpld;
        for (j = 0; j < w; j++) {
            rval = buf2r[j] / 64;
            gval = buf2g[j] / 64;
            bval = buf2b[j] / 64;
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            octreeFindColorCell(octindex, cqcaa, &index, &rc, &gc, &bc);
            SET_DATA_BYTE(lined, j, index);
        }

buffer_cleanup:
        LEPT_FREE(bufu8r);
        LEPT_FREE(bufu8g);
        LEPT_FREE(bufu8b);
        LEPT_FREE(buf1r);
        LEPT_FREE(buf1g);
        LEPT_FREE(buf1b);
        LEPT_FREE(buf2r);
        LEPT_FREE(buf2g);
        LEPT_FREE(buf2b);
        if (!success) pixDestroy(&pixd);
    }

    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);
    return pixd;
}