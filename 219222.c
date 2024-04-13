octreeFindColorCell(l_int32    octindex,
                    CQCELL  ***cqcaa,
                    l_int32   *pindex,
                    l_int32   *prval,
                    l_int32   *pgval,
                    l_int32   *pbval)
{
l_int32  level;
l_int32  baseindex, subindex;
CQCELL  *cqc, *cqcsub;

        /* Use rgb values stored in the cubes; a little faster */
    for (level = 2; level < CqNLevels; level++) {
        getOctcubeIndices(octindex, level, &baseindex, &subindex);
        cqc = cqcaa[level][baseindex];
        cqcsub = cqcaa[level + 1][subindex];
        if (cqcsub->bleaf == 0) {  /* use cell at level above */
            *pindex = cqc->index;
            *prval = cqc->rc;
            *pgval = cqc->gc;
            *pbval = cqc->bc;
            break;
        } else if (level == CqNLevels - 1) {  /* reached the bottom */
            *pindex = cqcsub->index;
            *prval = cqcsub->rc;
            *pgval = cqcsub->gc;
            *pbval = cqcsub->bc;
             break;
        }
    }

#if 0
        /* Generate rgb values for each cube on the fly; slower */
    for (level = 2; level < CqNLevels; level++) {
        l_int32  rv, gv, bv;
        getOctcubeIndices(octindex, level, &baseindex, &subindex);
        cqc = cqcaa[level][baseindex];
        cqcsub = cqcaa[level + 1][subindex];
        if (cqcsub->bleaf == 0) {  /* use cell at level above */
            getRGBFromOctcube(baseindex, level, &rv, &gv, &bv);
            *pindex = cqc->index;
            *prval = rv;
            *pgval = gv;
            *pbval = bv;
            break;
        } else if (level == CqNLevels - 1) {  /* reached the bottom */
            getRGBFromOctcube(subindex, level + 1, &rv, &gv, &bv);
           *pindex = cqcsub->index;
            *prval = rv;
            *pgval = gv;
            *pbval = bv;
            break;
        }
    }
#endif

    return 0;
}