static void dbReleaseColumnNames(DbEvalContext *p) {
    //Jsi_Interp *interp = p->jdb->interp;

    if( p->apColName && p->apColName != (char**)p->staticColNames) {
        int i;
        for(i=0; i<p->nCol; i++) {
            Jsi_Free(p->apColName[i]);
        }
        Jsi_Free((char *)p->apColName);
    }
    if( p->apColType && p->apColType != p->staticColTypes) {
        Jsi_Free((char *)p->apColType);
    }
    p->apColName = NULL;
    p->apColType = NULL;
    p->nCol = 0;
}