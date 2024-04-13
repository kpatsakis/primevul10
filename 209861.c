static Jsi_RC dbPrepareAndBind(
    Jsi_Db *jdb,                  /* Database object */
    char const *zIn,                /* SQL to compile */
    char const **pzOut,             /* OUT: Pointer to next SQL statement */
    SqlPreparedStmt **ppPreStmt     /* OUT: Object used to cache statement */
) {
    if (dbPrepareStmt(jdb, zIn, pzOut, ppPreStmt) != JSI_OK)
        return JSI_ERROR;
    return dbBindStmt(jdb, *ppPreStmt);
}