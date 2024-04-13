getOctcubeIndexFromRGB(l_int32    rval,
                       l_int32    gval,
                       l_int32    bval,
                       l_uint32  *rtab,
                       l_uint32  *gtab,
                       l_uint32  *btab,
                       l_uint32  *pindex)
{
    *pindex = rtab[rval] | gtab[gval] | btab[bval];
    return;
}