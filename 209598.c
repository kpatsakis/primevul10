static Jsi_RC RegExp_constructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Value *target;
    
    if (Jsi_FunctionIsConstructor(funcPtr))
        target = _this;
    else {
        Jsi_Obj *o = Jsi_ObjNewType(interp, JSI_OT_REGEXP);
        Jsi_ValueMakeObject(interp, ret, o);
        target = *ret;
    }
    
    Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
    const char *regtxt = "";
    const char *mods = NULL;
    if (v) {
        if (v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_REGEXP) {
            Jsi_ValueCopy(interp,target, v);
            return JSI_OK;
        } else if (!(regtxt = Jsi_ValueString(interp, v, NULL))) {
            return JSI_ERROR;
        }
    }
    Jsi_Value *f = Jsi_ValueArrayIndex(interp, args, 1);
    if (f)
        mods = Jsi_ValueString(interp, f, NULL);
    Jsi_DString dStr = {};
    Jsi_DSAppend(&dStr, "/", regtxt, "/", mods, NULL);
    Jsi_Regex *re = Jsi_RegExpNew(interp, Jsi_DSValue(&dStr), 0);
    Jsi_DSFree(&dStr);
    if (re == NULL)
        return JSI_ERROR;
    target->d.obj->d.robj = re;
    target->d.obj->ot = JSI_OT_REGEXP;
    return JSI_OK;
}