Jsi_TreeEntry * Jsi_ObjInsert(Jsi_Interp *interp, Jsi_Obj *obj, const char *key, Jsi_Value *val, int flags)
{
    Jsi_TreeEntry *hPtr;
    SIGASSERT(val, VALUE);
    /*if (val)
        Jsi_IncrRefCount(interp, val);*/
    hPtr = Jsi_TreeObjSetValue(obj, key, val, (flags&JSI_OM_ISSTRKEY));
    if ((flags&JSI_OM_DONTDEL))
        val->f.bits.dontdel = hPtr->f.bits.dontdel = 1;
    if ((flags&JSI_OM_READONLY))
        val->f.bits.readonly =hPtr->f.bits.readonly = 1;
    if ((flags&JSI_OM_DONTENUM))
        val->f.bits.dontenum =hPtr->f.bits.dontenum = 1;
    return hPtr;
}