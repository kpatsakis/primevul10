static void dbFlushStmtCache( Jsi_Db *jdb ) {
    Jsi_ListClear(jdb->stmtCache);
    jdb->stmtCacheCnt = 0;
}