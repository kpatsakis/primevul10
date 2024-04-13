static void dbUpdateHandler(
    void *p,
    int op,
    const char *zDb,
    const char *zTbl,
    sqlite_int64 rowid
) {
    Jsi_Db *jdb = (Jsi_Db *)p;
    Jsi_Interp *interp = jdb->interp;
    int rc, i = 0;
    Jsi_Value *vpargs, *items[10] = {}, *ret;
    
    assert( op==SQLITE_INSERT || op==SQLITE_UPDATE || op==SQLITE_DELETE );
    items[i++] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, (op==SQLITE_INSERT)?"INSERT":(op==SQLITE_UPDATE)?"UPDATE":"DELETE");
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zDb);
    items[i++] = Jsi_ValueMakeStringDup(interp, NULL, zTbl);
    items[i++] = Jsi_ValueMakeNumber(interp, NULL, (Jsi_Number)rowid);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, i, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onUpdate, vpargs, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK)
        jdb->errCnt++;
}