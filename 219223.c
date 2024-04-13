cqcellTreeDestroy(CQCELL  ****pcqcaa)
{
l_int32    level, ncells, i;
CQCELL  ***cqcaa;
CQCELL   **cqca;

    PROCNAME("cqcellTreeDestroy");

    if (pcqcaa == NULL) {
        L_WARNING("ptr address is NULL\n", procName);
        return;
    }

    if ((cqcaa = *pcqcaa) == NULL)
        return;

    for (level = 0; level <= CqNLevels; level++) {
        cqca = cqcaa[level];
        ncells = 1 << (3 * level);
        for (i = 0; i < ncells; i++)
            LEPT_FREE(cqca[i]);
        LEPT_FREE(cqca);
    }
    LEPT_FREE(cqcaa);
    *pcqcaa = NULL;

    return;
}