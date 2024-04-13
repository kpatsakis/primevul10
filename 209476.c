jsi_wsFileUploadCB(void *data, const char *name, const char *filename,
                   char *buf, int len, enum lws_spa_fileupload_states state)
{
    jsi_wsPss *pss = (typeof(pss))data;
    jsi_wsCmdObj *cmdPtr = pss->cmdPtr;
    Jsi_Value* callPtr = (pss->onUpload?pss->onUpload:cmdPtr->onUpload);
    Jsi_Interp *interp = cmdPtr->interp;
    const char *str;
    int slen, n = 0;
    if (cmdPtr->deleted) return -1;

    Jsi_Obj *oarg1;
    Jsi_Value *vpargs, *vargs[10];
    if (state == LWS_UFS_OPEN)
        pss->file_length = 0;
    //id:number, filename:string, data:string, startpos:number, complete:boolean
    vargs[n++] = Jsi_ValueNewObj(interp, cmdPtr->fobj);
    vargs[n++] = Jsi_ValueNewNumber(interp, (Jsi_Number)(pss->wid));
    vargs[n++] = Jsi_ValueNewBlobString(interp, filename);
    vargs[n++] = Jsi_ValueNewBlob(interp, (uchar*)buf, (uint)len);
    vargs[n++] = Jsi_ValueNewNumber(interp, (Jsi_Number)pss->file_length);
    vargs[n++] = Jsi_ValueNewBoolean(interp, (state==LWS_UFS_FINAL_CONTENT));
    vpargs = Jsi_ValueMakeObject(interp, NULL, oarg1 = Jsi_ObjNewArray(interp, vargs, n, 0));
    Jsi_IncrRefCount(interp, vpargs);
    pss->file_length += len;

    Jsi_Value *ret = Jsi_ValueNew1(interp);
    Jsi_ValueMakeUndef(interp, &ret);
    Jsi_RC rc = Jsi_FunctionInvoke(interp, callPtr, vpargs, &ret, NULL);

    if ((state==LWS_UFS_FINAL_CONTENT || rc != JSI_OK) && (str=Jsi_ValueString(interp, ret, &slen))) {
        Jsi_DSAppendLen(&pss->resultStr, str, slen);
        pss->resultCode = rc;
    }

    Jsi_DecrRefCount(interp, vpargs);
    Jsi_DecrRefCount(interp, ret);
    if (rc != JSI_OK) {
        Jsi_LogError("websock bad rcv eval");
        return -1;
    }
    return 0;
}