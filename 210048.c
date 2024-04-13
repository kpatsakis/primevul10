static void dbPrepStmtFree( Jsi_Db *jdb, SqlPreparedStmt *prep)
{
    if (prep->deleting)
        return;
    prep->deleting = 1;
    if (prep->pStmt)
        sqlite3_finalize( prep->pStmt );
    if (prep->entry) {
        Jsi_HashEntry *hPtr = prep->entry;
        prep->entry = NULL;
        Jsi_HashEntryDelete(hPtr);
    }
    if (prep->elPtr)
        Jsi_ListEntryDelete(prep->elPtr);
    Jsi_Free( (char*)prep );
    jdb->stmtCacheCnt--;
}