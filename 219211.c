cqcellTreeCreate(void)
{
l_int32    level, ncells, i;
CQCELL  ***cqcaa;
CQCELL   **cqca;   /* one array for each octree level */

    PROCNAME("cqcellTreeCreate");

        /* Make array of accumulation cell arrays from levels 1 to 5 */
    if ((cqcaa = (CQCELL ***)LEPT_CALLOC(CqNLevels + 1, sizeof(CQCELL **)))
        == NULL)
        return (CQCELL ***)ERROR_PTR("cqcaa not made", procName, NULL);
    for (level = 0; level <= CqNLevels; level++) {
        ncells = 1 << (3 * level);
        if ((cqca = (CQCELL **)LEPT_CALLOC(ncells, sizeof(CQCELL *))) == NULL) {
            cqcellTreeDestroy(&cqcaa);
            return (CQCELL ***)ERROR_PTR("cqca not made", procName, NULL);
        }
        cqcaa[level] = cqca;
        for (i = 0; i < ncells; i++) {
            if ((cqca[i] = (CQCELL *)LEPT_CALLOC(1, sizeof(CQCELL))) == NULL) {
                cqcellTreeDestroy(&cqcaa);
                return (CQCELL ***)ERROR_PTR("cqc not made", procName, NULL);
            }
        }
    }

    return cqcaa;
}