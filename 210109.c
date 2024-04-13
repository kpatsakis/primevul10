static void mdbStmtLimit( MySqlObj *jdb)
{
    while(jdb->numStmts>jdb->maxStmts ) {
        Jsi_ListEntry *l = Jsi_ListPopBack(jdb->stmtCache);
        mdbRelease1Stmt(jdb, (MysqlPrep*)Jsi_ListValueGet(l));
        jdb->numStmts = Jsi_ListSize(jdb->stmtCache);
    }
}