static void dbRollbackHandler(void *cd) {
    Jsi_Db *jdb = (Jsi_Db*)cd;
    Jsi_Interp *interp = jdb->interp;
    Jsi_Value *vpargs, *items[2] = {}, *ret = Jsi_ValueNew1(interp);
    
    items[0] = Jsi_ValueNewObj(interp, jdb->fobj);
    vpargs = Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNewArray(interp, items, 1, 0));
    Jsi_IncrRefCount(interp, vpargs);
    Jsi_FunctionInvoke(interp, jdb->onRollback, NULL, &ret, NULL);
    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
}