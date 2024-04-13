Jsi_RC jsi_RegExpValueNew(Jsi_Interp *interp, const char *regtxt, Jsi_Value *ret)
{
    
    Jsi_DString dStr = {};
    Jsi_DSAppend(&dStr, "/", regtxt, "/", NULL);
    Jsi_Regex *re = Jsi_RegExpNew(interp, Jsi_DSValue(&dStr), 0);
    Jsi_DSFree(&dStr);
    if (re == NULL)
        return JSI_ERROR;
    Jsi_Obj *o = Jsi_ObjNewType(interp, JSI_OT_REGEXP);
    Jsi_ValueMakeObject(interp, &ret, o);
    ret->d.obj->d.robj = re;
    ret->d.obj->ot = JSI_OT_REGEXP;
    return JSI_OK;
}