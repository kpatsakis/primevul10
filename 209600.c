static Jsi_RC WebSocketUnaliasCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    jsi_wsCmdObj *cmdPtr = (jsi_wsCmdObj*)Jsi_UserObjGetData(interp, _this, funcPtr);
    if (!cmdPtr)
        return Jsi_LogError("Apply to non-websock object");
    int vlen, nlen;
    const char *kstr, *vstr, *nstr = Jsi_ValueArrayIndexToStr(interp, args, 0, &nlen);
    if (!nstr)
        return Jsi_LogError("arg 1: expected string");
    Jsi_Value *v, *a = cmdPtr->pathAliases;
    if (!a|| !Jsi_ValueIsObjType(interp, a, JSI_OT_OBJECT)) return JSI_OK;
    Jsi_IterObj *io = Jsi_IterObjNew(interp, NULL);
    Jsi_IterGetKeys(interp, cmdPtr->pathAliases, io, 0);
    uint i;
    for (i=0; i<io->count; i++) {
        kstr = io->keys[i];
        v = Jsi_ValueObjLookup(interp, a, kstr, 1);
        if (!v) continue;
        vstr = Jsi_ValueToString(interp, v, &vlen);
        if (!vstr) continue;
        if (nlen<=vlen) continue;
        if (Jsi_Strncmp(vstr, nstr, vlen)) continue;
        Jsi_DString dStr = {};
        Jsi_DSAppend(&dStr, "/", kstr, nstr+vlen, NULL);
        Jsi_ValueFromDS(interp, &dStr, ret);
        break;
    }
    Jsi_IterObjFree(io);
    return JSI_OK;
}