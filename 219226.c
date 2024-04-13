getOctcubeIndices(l_int32   rgbindex,
                  l_int32   level,
                  l_int32  *pbindex,
                  l_int32  *psindex)
{
    PROCNAME("getOctcubeIndex");

    if (level < 0 || level > CqNLevels - 1)
        return ERROR_INT("level must be in e.g., [0 ... 5]", procName, 1);
    if (!pbindex)
        return ERROR_INT("&bindex not defined", procName, 1);
    if (!psindex)
        return ERROR_INT("&sindex not defined", procName, 1);

    *pbindex = rgbindex >> (3 * (CqNLevels - level));
    *psindex = rgbindex >> (3 * (CqNLevels - 1 - level));
    return 0;
}