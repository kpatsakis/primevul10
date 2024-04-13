static void mdbDeleteCmd(MySqlObj *jdb)
{
    Jsi_Interp *interp = jdb->interp;
    if (jdb->debug & mdbTMODE_DELETE)
        JSI_DBQUERY_PRINTF( "DEBUG: delete\n");
    if (jdb->stmtCache)
        mdbFlushStmtCache(jdb);
    if (jdb->stmtHash)
        Jsi_HashDelete(jdb->stmtHash);
    //closeIncrblobChannels(jdb);
    if (jdb->db) {
        mdbClose(jdb->db);
    }
    while( jdb->pFunc ) {
        SqlFunc *pFunc = jdb->pFunc;
        jdb->pFunc = pFunc->pNext;
        Jsi_DSFree(&pFunc->dScript);
        Jsi_DecrRefCount(interp, pFunc->tocall);
        Jsi_Free((char*)pFunc);
    }
    if( jdb->zNull ) {
        Jsi_Free(jdb->zNull);
    }
    if (jdb->typeNameHash)
        Jsi_HashDelete(jdb->typeNameHash);
    Jsi_OptionsFree(interp, SqlOptions, jdb, 0);
    if (jdb->stmtCache)
        Jsi_ListDelete(jdb->stmtCache);
}