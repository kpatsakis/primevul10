static void dbCollateNeeded(
    void *cd,
    sqlite3 *db,
    int enc,
    const char *zName
) {
    int rc;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *vpargs, *items[2], *ret;
    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[1] = Jsi_ValueMakeStringDup(interp, NULL, zName);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 2, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onNeedCollate, vpargs,& ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK)
        jdb->errCnt++;

}