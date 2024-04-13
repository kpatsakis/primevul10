static void mdbFlushStmtCache( MySqlObj *jdb ) {
    Jsi_ListClear(jdb->stmtCache);
    jdb->numStmts = 0;
}