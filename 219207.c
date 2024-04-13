octcubeGetCount(l_int32   level,
                l_int32  *psize)
{
    PROCNAME("octcubeGetCount");

    if (!psize)
        return ERROR_INT("&size not defined", procName, 1);
    if (level < 1 || level > 6)
        return ERROR_INT("invalid level", procName, 1);

    *psize = 1 << (3 * level);
    return 0;
}