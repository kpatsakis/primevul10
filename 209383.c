static void dbPrepStmtLimit( Jsi_Db *jdb)
{
    while(jdb->stmtCacheCnt>jdb->stmtCacheMax ) {
        Jsi_ListEntry *l = Jsi_ListPopBack(jdb->stmtCache);
        dbPrepStmtFree(jdb, (SqlPreparedStmt*)Jsi_ListValueGet(l));
        jdb->stmtCacheCnt = Jsi_ListSize(jdb->stmtCache);
    }
}