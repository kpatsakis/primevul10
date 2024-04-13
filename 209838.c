static void dbDeleteCmd(Jsi_Db *jdb)
{
    Jsi_Interp *interp = jdb->interp;
    if (jdb->debug & TMODE_DELETE)
        JSI_DBQUERY_PRINTF( "DEBUG: delete\n");
    dbFlushStmtCache(jdb);
    if (jdb->stmtHash)
        Jsi_HashDelete(jdb->stmtHash);
    //closeIncrblobChannels(jdb);
    if (jdb->db) {
        DbClose(jdb->db);
    }
    while( jdb->pFunc ) {
        SqlFunc *pFunc = jdb->pFunc;
        jdb->pFunc = pFunc->pNext;
        Jsi_DSFree(&pFunc->dScript);
        Jsi_DecrRefCount(interp, pFunc->tocall);
        Jsi_Free((char*)pFunc);
    }
    while( jdb->pCollate ) {
        SqlCollate *pCollate = jdb->pCollate;
        jdb->pCollate = pCollate->pNext;
        Jsi_Free((char*)pCollate);
    }

    Jsi_OptionsFree(interp, SqlOptions, jdb, 0);
    if (jdb->stmtCache)
        Jsi_ListDelete(jdb->stmtCache);
}