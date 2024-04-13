static Jsi_RC NumberConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    if (Jsi_FunctionIsConstructor(funcPtr)) {
        Jsi_Number nv = 0.0;
        Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
        if (v) {
            Jsi_ValueToNumber(interp, v);
            nv = v->d.num;
        }
        _this->d.obj->ot = JSI_OT_NUMBER;
        _this->d.obj->d.num = nv;
        Jsi_ValueToObject(interp, _this);
        Jsi_ValueMakeNumber(interp, ret, nv);
        return JSI_OK;
    }
    Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
    if (v) {
        Jsi_ValueToNumber(interp, v);
        Jsi_ValueDup2(interp, ret, v);
        Jsi_ValueToObject(interp, *ret);
        return JSI_OK;
    }
    Jsi_ValueMakeNumber(interp, ret, 0.0);
    return JSI_OK;
}