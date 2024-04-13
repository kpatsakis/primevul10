static void jsi_wsOnModify(jsi_wsCmdObj *cmdPtr) {
    if (!cmdPtr->stats.httpLast) return;
    time_t now = time(NULL);
    double dt = (difftime(now, cmdPtr->lastModifyCheck));
    if (dt<0) {
        cmdPtr->lastModifyCheck = now;
        return;
    }
    uint secs = cmdPtr->modifySecs;
    if (!secs) secs = 2;
    if (dt<secs) return;
    cmdPtr->lastModifyCheck = now;
    Jsi_Interp *interp = cmdPtr->interp;
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch cursor;
    Jsi_Value* changed = NULL;
    time_t ll = cmdPtr->lastModifyNotify;
    if (ll<cmdPtr->stats.httpLast)
        ll = cmdPtr->stats.httpLast;
    for (hPtr = Jsi_HashSearchFirst(cmdPtr->fileHash, &cursor);
        hPtr != NULL; hPtr = Jsi_HashSearchNext(&cursor)) {
        jsi_wsFile* fPtr = (jsi_wsFile*)Jsi_HashValueGet(hPtr);
        if (fPtr && fPtr->fileVal) {
            Jsi_StatBuf sb;
            int n = Jsi_Stat(interp, fPtr->fileVal, &sb);
            if (!n && sb.st_mtime > ll) {
                changed = fPtr->fileVal;
                break;
            }
        }
    }
    cmdPtr->lastModifyCheck = time(NULL);
    if (!changed) return;
    Jsi_Obj *oarg1;
    Jsi_Value *vpargs, *vargs[10];
    int n = 0;
    vargs[n++] = Jsi_ValueNewObj(interp, cmdPtr->fobj);
    vargs[n++] = changed;
    vpargs = Jsi_ValueMakeObject(interp, NULL, oarg1 = Jsi_ObjNewArray(interp, vargs, n, 0));
    Jsi_IncrRefCount(interp, vpargs);

    Jsi_Value *ret = Jsi_ValueNew1(interp);
    Jsi_ValueMakeUndef(interp, &ret);
    Jsi_RC rc = Jsi_FunctionInvoke(interp, cmdPtr->onModify, vpargs, &ret, NULL);

    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK) {
        Jsi_LogWarn("websock bad onModify eval: disabling");
        Jsi_DecrRefCount(interp, cmdPtr->onModify);
        cmdPtr->onModify = NULL;
    }
    cmdPtr->lastModifyCheck = time(NULL);
    cmdPtr->lastModifyNotify = time(NULL);
}