static Jsi_RC WebSocketSendCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply in a non-websock object");
    WSSIGASSERT(cmdPtr, OBJ);
    jsi_wsPss *pss;
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch cursor;
    Jsi_Value *arg = Jsi_ValueArrayIndex(interp, args, 1);
    char *str = Jsi_ValueString(interp, arg, NULL);
    int id = -1, argc = Jsi_ValueGetLength(interp, args);
    Jsi_DString eStr = {};
    if (argc!=2)
        return Jsi_LogError("wrong args");
    Jsi_Number dnum;
    Jsi_Value *darg = Jsi_ValueArrayIndex(interp, args, 0);
    if (Jsi_ValueGetNumber(interp, darg, &dnum) != JSI_OK)
        return Jsi_LogError("invalid id");
    id = (int)dnum;

    if (!str)
        str = (char*)Jsi_ValueGetDString(interp, arg, &eStr, JSI_OUTPUT_JSON);

    if (cmdPtr->echo)
        Jsi_LogInfo("WS-SEND: %s\n", str);

    for (hPtr = Jsi_HashSearchFirst(cmdPtr->pssTable, &cursor);
        hPtr != NULL; hPtr = Jsi_HashSearchNext(&cursor)) {
        pss = (jsi_wsPss*)Jsi_HashValueGet(hPtr);
        WSSIGASSERT(pss, PWS);
        if ((id<0 || pss->wid == id) && pss->state != PWS_DEAD) {
            if (!pss->stack)
                pss->stack = Jsi_StackNew();
            char *msg = (char*)Jsi_Malloc(LWS_PRE + Jsi_Strlen(str) + 1);
            Jsi_Strcpy(msg + LWS_PRE, str);
            Jsi_StackPush(pss->stack, msg);
            pss->stats.msgQLen++;
            if (!cmdPtr->echo && pss->echo)
                Jsi_LogInfo("WS-SEND: %s\n", str);
        }
    }

    Jsi_DSFree(&eStr);
    return JSI_OK;
}