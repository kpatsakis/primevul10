octreeGenerateAndPrune(PIX       *pixs,
                       l_int32    colors,
                       l_int32    reservedcolors,
                       PIXCMAP  **pcmap)
{
l_int32    rval, gval, bval, cindex;
l_int32    level, ncells, octindex;
l_int32    w, h, wpls;
l_int32    i, j, isub;
l_int32    npix;  /* number of remaining pixels to be assigned */
l_int32    ncolor; /* number of remaining color cells to be used */
l_int32    ppc;  /* ave number of pixels left for each color cell */
l_int32    rv, gv, bv;
l_float32  thresholdFactor[] = {0.01f, 0.01f, 1.0f, 1.0f, 1.0f, 1.0f};
l_float32  thresh;  /* factor of ppc for this level */
l_uint32  *datas, *lines;
l_uint32  *rtab, *gtab, *btab;
CQCELL  ***cqcaa;   /* one array for each octree level */
CQCELL   **cqca, **cqcasub;
CQCELL    *cqc, *cqcsub;
PIXCMAP   *cmap;
NUMA      *nat;  /* accumulates levels for threshold cells */
NUMA      *nar;  /* accumulates levels for residual cells */

    PROCNAME("octreeGenerateAndPrune");

    if (!pixs)
        return (CQCELL ***)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (CQCELL ***)ERROR_PTR("pixs must be 32 bpp", procName, NULL);
    if (colors < 128 || colors > 256)
        return (CQCELL ***)ERROR_PTR("colors not in [128,256]", procName, NULL);
    if (!pcmap)
        return (CQCELL ***)ERROR_PTR("&cmap not defined", procName, NULL);

    if ((cqcaa = cqcellTreeCreate()) == NULL)
        return (CQCELL ***)ERROR_PTR("cqcaa not made", procName, NULL);

        /* Make the canonical index tables */
    rtab = gtab = btab = NULL;
    makeRGBToIndexTables(CqNLevels, &rtab, &gtab, &btab);

        /* Generate an 8 bpp cmap (max size 256) */
    cmap = pixcmapCreate(8);
    *pcmap = cmap;

    pixGetDimensions(pixs, &w, &h, NULL);
    npix = w * h;  /* initialize to all pixels */
    ncolor = colors - reservedcolors - ExtraReservedColors;
    ppc = npix / ncolor;
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);

        /* Accumulate the centers of each cluster at level CqNLevels */
    ncells = 1 << (3 * CqNLevels);
    cqca = cqcaa[CqNLevels];
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        for (j = 0; j < w; j++) {
            extractRGBValues(lines[j], &rval, &gval, &bval);
            octindex = rtab[rval] | gtab[gval] | btab[bval];
            cqc = cqca[octindex];
            cqc->n++;
        }
    }

        /* Arrays for storing statistics */
    nat = numaCreate(0);
    nar = numaCreate(0);

        /* Prune back from the lowest level and generate the colormap */
    for (level = CqNLevels - 1; level >= 2; level--) {
        thresh = thresholdFactor[level];
        cqca = cqcaa[level];
        cqcasub = cqcaa[level + 1];
        ncells = 1 << (3 * level);
        for (i = 0; i < ncells; i++) {  /* i is octindex at level */
            cqc = cqca[i];
            for (j = 0; j < 8; j++) {  /* check all subnodes */
                isub = 8 * i + j;   /* isub is octindex at level+1 */
                cqcsub = cqcasub[isub];
                if (cqcsub->bleaf == 1) {  /* already a leaf? */
                    cqc->nleaves++;   /* count the subcube leaves */
                    continue;
                }
                if (cqcsub->n >= thresh * ppc) {  /* make it a true leaf? */
                    cqcsub->bleaf = 1;
                    if (cmap->n < 256) {
                        cqcsub->index = cmap->n;  /* assign the color index */
                        getRGBFromOctcube(isub, level + 1, &rv, &gv, &bv);
                        pixcmapAddColor(cmap, rv, gv, bv);
#if 1   /* save values */
                        cqcsub->rc = rv;
                        cqcsub->gc = gv;
                        cqcsub->bc = bv;
#endif
                    } else {
                            /* This doesn't seem to happen. Do something. */
                        L_ERROR("assigning pixels to wrong color\n", procName);
                        pixcmapGetNearestIndex(cmap, 128, 128, 128, &cindex);
                        cqcsub->index = cindex;  /* assign to the nearest */
                        pixcmapGetColor(cmap, cindex, &rval, &gval, &bval);
                        cqcsub->rc = rval;
                        cqcsub->gc = gval;
                        cqcsub->bc = bval;
                    }
                    cqc->nleaves++;
                    npix -= cqcsub->n;
                    ncolor--;
                    if (ncolor > 0)
                        ppc = npix / ncolor;
                    else if (ncolor + reservedcolors > 0)
                        ppc = npix / (ncolor + reservedcolors);
                    else
                        ppc = 1000000;  /* make it big */
                    numaAddNumber(nat, level + 1);

#if  DEBUG_OCTCUBE_CMAP
    lept_stderr("Exceeds threshold: colors used = %d, colors remaining = %d\n",
                cmap->n, ncolor + reservedcolors);
    lept_stderr("  cell with %d pixels, npix = %d, ppc = %d\n",
                cqcsub->n, npix, ppc);
    lept_stderr("  index = %d, level = %d, subindex = %d\n",
                i, level, j);
    lept_stderr("  rv = %d, gv = %d, bv = %d\n", rv, gv, bv);
#endif  /* DEBUG_OCTCUBE_CMAP */

                }
            }
            if (cqc->nleaves > 0 || level == 2) { /* make the cube a leaf now */
                cqc->bleaf = 1;
                if (cqc->nleaves < 8) {  /* residual CTE cube: acquire the
                                          * remaining pixels */
                    for (j = 0; j < 8; j++) {  /* check all subnodes */
                        isub = 8 * i + j;
                        cqcsub = cqcasub[isub];
                        if (cqcsub->bleaf == 0)  /* absorb */
                            cqc->n += cqcsub->n;
                    }
                    if (cmap->n < 256) {
                        cqc->index = cmap->n;  /* assign the color index */
                        getRGBFromOctcube(i, level, &rv, &gv, &bv);
                        pixcmapAddColor(cmap, rv, gv, bv);
#if 1   /* save values */
                        cqc->rc = rv;
                        cqc->gc = gv;
                        cqc->bc = bv;
#endif
                    } else {
                        L_WARNING("possibly assigned pixels to wrong color\n",
                                  procName);
                            /* This is very bad.  It will only cause trouble
                             * with dithering, and we try to avoid it with
                             * ExtraReservedColors. */
                        pixcmapGetNearestIndex(cmap, rv, gv, bv, &cindex);
                        cqc->index = cindex;  /* assign to the nearest */
                        pixcmapGetColor(cmap, cindex, &rval, &gval, &bval);
                        cqc->rc = rval;
                        cqc->gc = gval;
                        cqc->bc = bval;
                    }
                    npix -= cqc->n;
                    ncolor--;
                    if (ncolor > 0)
                        ppc = npix / ncolor;
                    else if (ncolor + reservedcolors > 0)
                        ppc = npix / (ncolor + reservedcolors);
                    else
                        ppc = 1000000;  /* make it big */
                    numaAddNumber(nar, level);

#if  DEBUG_OCTCUBE_CMAP
    lept_stderr("By remainder: colors used = %d, colors remaining = %d\n",
                cmap->n, ncolor + reservedcolors);
    lept_stderr("  cell with %d pixels, npix = %d, ppc = %d\n",
                cqc->n, npix, ppc);
    lept_stderr("  index = %d, level = %d\n", i, level);
    lept_stderr("  rv = %d, gv = %d, bv = %d\n", rv, gv, bv);
#endif  /* DEBUG_OCTCUBE_CMAP */

                }
            } else {  /* absorb all the subpixels but don't make it a leaf */
                for (j = 0; j < 8; j++) {  /* absorb from all subnodes */
                    isub = 8 * i + j;
                    cqcsub = cqcasub[isub];
                    cqc->n += cqcsub->n;
                }
            }
        }
    }

#if  PRINT_OCTCUBE_STATS
{
l_int32    tc[] = {0, 0, 0, 0, 0, 0, 0};
l_int32    rc[] = {0, 0, 0, 0, 0, 0, 0};
l_int32    nt, nr, ival;

    nt = numaGetCount(nat);
    nr = numaGetCount(nar);
    for (i = 0; i < nt; i++) {
        numaGetIValue(nat, i, &ival);
        tc[ival]++;
    }
    for (i = 0; i < nr; i++) {
        numaGetIValue(nar, i, &ival);
        rc[ival]++;
    }
    lept_stderr(" Threshold cells formed: %d\n", nt);
    for (i = 1; i < CqNLevels + 1; i++)
        lept_stderr("   level %d:  %d\n", i, tc[i]);
    lept_stderr("\n Residual cells formed: %d\n", nr);
    for (i = 0; i < CqNLevels ; i++)
        lept_stderr("   level %d:  %d\n", i, rc[i]);
}
#endif  /* PRINT_OCTCUBE_STATS */

    numaDestroy(&nat);
    numaDestroy(&nar);
    LEPT_FREE(rtab);
    LEPT_FREE(gtab);
    LEPT_FREE(btab);

    return cqcaa;
}