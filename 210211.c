static void dbProfileHandler(void *cd, const char *zSql, sqlite_uint64 tm) {
    int rc;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *vpargs, *items[3] = {}, *ret;

    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[1] = Jsi_ValueMakeStringDup(interp, NULL, zSql);
    items[2] = Jsi_ValueMakeNumber(interp, NULL, (Jsi_Number)tm);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 3, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onProfile, vpargs, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK)
        jdb->errCnt++;
}