pixFixedOctcubeQuant256(PIX     *pixs,
                        l_int32  ditherflag)
{
l_uint8    index;
l_int32    rval, gval, bval;
l_int32    w, h, wpls, wpld, i, j, cindex;
l_uint32  *rtab, *gtab, *btab;
l_int32   *itab;
l_uint32  *datas, *datad, *lines, *lined;
PIX       *pixd;
PIXCMAP   *cmap;

    PROCNAME("pixFixedOctcubeQuant256");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);

        /* Do not dither if image is very small */
    pixGetDimensions(pixs, &w, &h, NULL);
    if (w < MinDitherSize && h < MinDitherSize && ditherflag == 1) {
        L_INFO("Small image: dithering turned off\n", procName);
        ditherflag = 0;
    }

        /* Find the centers of the 256 cells, each of which represents
         * the 3 MSBits of the red and green components, and the
         * 2 MSBits of the blue component.  This gives a mapping
         * from a "cube index" to the rgb values.  Save all 256
         * rgb values of these centers in a colormap.
         * For example, to get the red color of the cell center,
         * you take the 3 MSBits of to the index and add the
         * offset to the center of the cell, which is 0x10. */
    cmap = pixcmapCreate(8);
    for (cindex = 0; cindex < 256; cindex++) {
        rval = (cindex & 0xe0) | 0x10;
        gval = ((cindex << 3) & 0xe0) | 0x10;
        bval = ((cindex << 6) & 0xc0) | 0x20;
        pixcmapAddColor(cmap, rval, gval, bval);
    }

        /* Make output 8 bpp palette image */
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    if ((pixd = pixCreate(w, h, 8)) == NULL) {
        pixcmapDestroy(&cmap);
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    }
    pixSetColormap(pixd, cmap);
    pixCopyResolution(pixd, pixs);
    pixCopyInputFormat(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);

        /* Set dest pix values to colortable indices */
    if (ditherflag == 0) {   /* no dithering */
        for (i = 0; i < h; i++) {
            lines = datas + i * wpls;
            lined = datad + i * wpld;
            for (j = 0; j < w; j++) {
                extractRGBValues(lines[j], &rval, &gval, &bval);
                index = (rval & 0xe0) | ((gval >> 3) & 0x1c) | (bval >> 6);
                SET_DATA_BYTE(lined, j, index);
            }
        }
    } else {  /* ditherflag == 1 */
            /* Set up conversion tables from rgb directly to the colormap
             * index.  However, the dithering function expects these tables
             * to generate an octcube index (+1), and the table itab[] to
             * convert to the colormap index.  So we make a trivial
             * itab[], that simply compensates for the -1 in
             * pixDitherOctindexWithCmap().   No cap is required on
             * the propagated difference.  */
        rtab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
        gtab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
        btab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
        itab = (l_int32 *)LEPT_CALLOC(256, sizeof(l_int32));
        if (!rtab || !gtab || !btab || !itab) {
            pixDestroy(&pixd);
            return (PIX *)ERROR_PTR("calloc fail for table", procName, NULL);
        }
        for (i = 0; i < 256; i++) {
            rtab[i] = i & 0xe0;
            gtab[i] = (i >> 3) & 0x1c;
            btab[i] = i >> 6;
            itab[i] = i + 1;
        }
        pixDitherOctindexWithCmap(pixs, pixd, rtab, gtab, btab, itab,
                                  FIXED_DIF_CAP);
        LEPT_FREE(rtab);
        LEPT_FREE(gtab);
        LEPT_FREE(btab);
        LEPT_FREE(itab);
    }

    return pixd;
}