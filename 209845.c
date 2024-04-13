static void mdbEvalFinalize(MyDbEvalContext *p) {
    if( p->prep) {
        mdbReleaseStmt(p->jdb, p->prep, p->nocache);
        p->prep = 0;
    }
    Jsi_DSFree(p->dzSql);
}