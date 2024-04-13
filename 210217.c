static SqlFunc *dbFindSqlFunc(Jsi_Db *jdb, const char *zName) {
    SqlFunc *p, *pNew;
    int i;
    pNew = (SqlFunc*)Jsi_Calloc(1, sizeof(*pNew) + Jsi_Strlen(zName) + 1 );
    pNew->sig = SQLITE_SIG_FUNC;
    pNew->zName = (char*)&pNew[1];
    for(i=0; zName[i]; i++) {
        pNew->zName[i] = tolower(zName[i]);
    }
    pNew->zName[i] = 0;
    for(p=jdb->pFunc; p; p=p->pNext) {
        if( Jsi_Strcmp(p->zName, pNew->zName)==0 ) {
            Jsi_Free((char*)pNew);
            return p;
        }
    }
    pNew->interp = jdb->interp;
    pNew->pScript = 0;
    Jsi_DSInit(&pNew->dScript);
    pNew->pNext = jdb->pFunc;
    jdb->pFunc = pNew;
    return pNew;
}