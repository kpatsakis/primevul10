static Jsi_RC StringFromCharCodeCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    if (_this->vt != JSI_VT_OBJECT || _this->d.obj->ot == JSI_OT_STRING)
        return Jsi_LogError("should be called via String.fromCharCode");
    
    Jsi_DString dStr = {};
    int n, i, len, argc = Jsi_ValueGetLength(interp, args);
    for (i=0; i<argc; i++) {
        Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, i);
        if (!Jsi_ValueIsNumber(interp, v) || Jsi_GetIntFromValue(interp, v, &n) != JSI_OK) {
            Jsi_DSFree(&dStr);
            return Jsi_LogError("expected int value at arg %d", i+1);
        }
        char dest[5];
        len = Jsi_UniCharToUtf((Jsi_UniChar)n, dest);
        Jsi_DSAppendLen(&dStr, dest, len);
    }
    
    Jsi_ValueMakeDStringObject(interp, ret, &dStr);
    return JSI_OK;
}