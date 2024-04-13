static Jsi_RC WebSocketIdCmdOp(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr, int op)
{
    Jsi_RC rc = JSI_OK;
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply in a non-websock object");
    Jsi_Value *v, *valPtr = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_Number vid;
    if (Jsi_ValueGetNumber(interp, valPtr, &vid) != JSI_OK || vid < 0)
        return Jsi_LogError("Expected connection number id");
    int id = (int)vid;
    jsi_wsPss *pss = NULL;
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch cursor;
    for (hPtr = Jsi_HashSearchFirst(cmdPtr->pssTable, &cursor);
        hPtr != NULL; hPtr = Jsi_HashSearchNext(&cursor)) {
        jsi_wsPss* tpss = (jsi_wsPss*)Jsi_HashValueGet(hPtr);
        WSSIGASSERT(tpss, PWS);
        if (tpss->wid == id && tpss->state != PWS_DEAD) {
            pss = tpss;
            break;
        }
    }

    if (!pss)
        return Jsi_LogError("No such id: %d", id);
    switch (op) {
        case 0:
            v = Jsi_ValueArrayIndex(interp, args, 1);
            rc = Jsi_OptionsConf(interp, WPSOptions, pss, v, ret, 0);
            break;
        case 1:
            jsi_wsDumpHeaders(cmdPtr, pss, Jsi_ValueArrayIndexToStr(interp, args, 1, NULL), ret);
            break;
        case 2:
            if (!pss->spa) return JSI_OK;
            jsi_wsDumpQuery(cmdPtr, pss, Jsi_ValueArrayIndexToStr(interp, args, 1, NULL), ret);
            break;
    }
    return rc;
}