makeRGBToIndexTables(l_int32     cqlevels,
                     l_uint32  **prtab,
                     l_uint32  **pgtab,
                     l_uint32  **pbtab)
{
l_int32    i;
l_uint32  *rtab, *gtab, *btab;

    PROCNAME("makeRGBToIndexTables");

    if (cqlevels < 1 || cqlevels > 6)
        return ERROR_INT("cqlevels must be in {1,...6}", procName, 1);
    if (!prtab || !pgtab || !pbtab)
        return ERROR_INT("not all &tabs defined", procName, 1);

    rtab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
    gtab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
    btab = (l_uint32 *)LEPT_CALLOC(256, sizeof(l_uint32));
    if (!rtab || !gtab || !btab)
        return ERROR_INT("calloc fail for tab", procName, 1);
    *prtab = rtab;
    *pgtab = gtab;
    *pbtab = btab;

    switch (cqlevels)
    {
    case 1:
        for (i = 0; i < 256; i++) {
            rtab[i] = (i >> 5) & 0x0004;
            gtab[i] = (i >> 6) & 0x0002;
            btab[i] = (i >> 7);
        }
        break;
    case 2:
        for (i = 0; i < 256; i++) {
            rtab[i] = ((i >> 2) & 0x0020) | ((i >> 4) & 0x0004);
            gtab[i] = ((i >> 3) & 0x0010) | ((i >> 5) & 0x0002);
            btab[i] = ((i >> 4) & 0x0008) | ((i >> 6) & 0x0001);
        }
        break;
    case 3:
        for (i = 0; i < 256; i++) {
            rtab[i] = ((i << 1) & 0x0100) | ((i >> 1) & 0x0020) |
                      ((i >> 3) & 0x0004);
            gtab[i] = (i & 0x0080) | ((i >> 2) & 0x0010) |
                      ((i >> 4) & 0x0002);
            btab[i] = ((i >> 1) & 0x0040) | ((i >> 3) & 0x0008) |
                      ((i >> 5) & 0x0001);
        }
        break;
    case 4:
        for (i = 0; i < 256; i++) {
            rtab[i] = ((i << 4) & 0x0800) | ((i << 2) & 0x0100) |
                      (i & 0x0020) | ((i >> 2) & 0x0004);
            gtab[i] = ((i << 3) & 0x0400) | ((i << 1) & 0x0080) |
                      ((i >> 1) & 0x0010) | ((i >> 3) & 0x0002);
            btab[i] = ((i << 2) & 0x0200) | (i & 0x0040) |
                      ((i >> 2) & 0x0008) | ((i >> 4) & 0x0001);
        }
        break;
    case 5:
        for (i = 0; i < 256; i++) {
            rtab[i] = ((i << 7) & 0x4000) | ((i << 5) & 0x0800) |
                      ((i << 3) & 0x0100) | ((i << 1) & 0x0020) |
                      ((i >> 1) & 0x0004);
            gtab[i] = ((i << 6) & 0x2000) | ((i << 4) & 0x0400) |
                      ((i << 2) & 0x0080) | (i & 0x0010) |
                      ((i >> 2) & 0x0002);
            btab[i] = ((i << 5) & 0x1000) | ((i << 3) & 0x0200) |
                      ((i << 1) & 0x0040) | ((i >> 1) & 0x0008) |
                      ((i >> 3) & 0x0001);
        }
        break;
    case 6:
        for (i = 0; i < 256; i++) {
            rtab[i] = ((i << 10) & 0x20000) | ((i << 8) & 0x4000) |
                      ((i << 6) & 0x0800) | ((i << 4) & 0x0100) |
                      ((i << 2) & 0x0020) | (i & 0x0004);
            gtab[i] = ((i << 9) & 0x10000) | ((i << 7) & 0x2000) |
                      ((i << 5) & 0x0400) | ((i << 3) & 0x0080) |
                      ((i << 1) & 0x0010) | ((i >> 1) & 0x0002);
            btab[i] = ((i << 8) & 0x8000) | ((i << 6) & 0x1000) |
                      ((i << 4) & 0x0200) | ((i << 2) & 0x0040) |
                      (i & 0x0008) | ((i >> 2) & 0x0001);
        }
        break;
    default:
        ERROR_INT("cqlevels not in [1...6]", procName, 1);
        break;
    }

    return 0;
}