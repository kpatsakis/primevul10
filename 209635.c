bool Jsi_ValueKeyPresent(Jsi_Interp *interp, Jsi_Value *target, const char *key, int isstrkey)
{
    SIGASSERT(interp,INTERP);
    //SIGASSERT(target,VALUE);
    if (Jsi_TreeObjGetValue(target->d.obj, key, isstrkey))
        return 1;
    if (target->d.obj->__proto__ == NULL || target->d.obj->__proto__ == target)
        return 0;
    return Jsi_ValueKeyPresent(interp, target->d.obj->__proto__, key, isstrkey);
}