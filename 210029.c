static void dbTraceHandler(void *cd, const char *zSql)
{
    int rc;
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Value *vpargs, *items[2] = {}, *ret;
    Jsi_Interp *interp = jdb->interp;
    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    items[1] = Jsi_ValueMakeStringDup(interp, NULL, zSql);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 2, 0));
    Jsi_IncrRefCount(interp, vpargs);
    ret = Jsi_ValueNew1(interp);
    rc = Jsi_FunctionInvoke(interp, jdb->onTrace, vpargs, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK)
        jdb->errCnt++;
}