static Jsi_Value *jsi_ValueLookupBase(Jsi_Interp *interp, Jsi_Value *target, Jsi_Value *key, Jsi_Value **ret)
{
    if (!target)
        return NULL;
    if (target->vt != JSI_VT_OBJECT) {
        Jsi_LogError("subscript operand is not object");
        return NULL;
    }
    const char *keyStr = Jsi_ValueToString(interp, key, NULL);
    if (!keyStr)
        return NULL;
    bool isStrKey = (key->vt == JSI_VT_STRING && key->f.bits.isstrkey);
    Jsi_Value *v = Jsi_ValueObjLookup(interp, target, (char*)keyStr, isStrKey);
    if (v)
        return v;
    if (target->d.obj->__proto__)
        return jsi_ValueLookupBase(interp, target->d.obj->__proto__, key, ret);
    return NULL;
}