static Jsi_RC WebSocketIdsCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply in a non-websock object");
    const char *val = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_DString dStr = {"["};
    jsi_wsPss *pss = NULL;
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch cursor;
    int cnt = 0;
    for (hPtr = Jsi_HashSearchFirst(cmdPtr->pssTable, &cursor);
        hPtr != NULL; hPtr = Jsi_HashSearchNext(&cursor)) {
        pss = (jsi_wsPss*)Jsi_HashValueGet(hPtr);
        WSSIGASSERT(pss, PWS);
        if (pss->state == PWS_DEAD) continue;
        if (val && Jsi_Strcmp(pss->key, val)) continue;
        Jsi_DSPrintf(&dStr, "%s%d", (cnt++?",":""), pss->wid);
        if (val) break;
    }
    Jsi_DSAppend(&dStr, "]", NULL);
    Jsi_RC rc = Jsi_JSONParse(interp, Jsi_DSValue(&dStr), ret, 0);
    Jsi_DSFree(&dStr);
    return rc;
}