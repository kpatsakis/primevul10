Jsi_Value *Jsi_ValueObjLookup(Jsi_Interp *interp, Jsi_Value *target, const char *key, int isstrkey)
{
    Jsi_Obj *obj;
    Jsi_Value *v = NULL;
    if (interp->subOpts.noproto && key) {
        if (key[0] == 'p' && Jsi_Strcmp(key, "prototype")==0) {
            Jsi_LogError("inheritance is disabled in interp");
            return NULL;
        }
    }
    if (target->vt != JSI_VT_OBJECT) {
        if (interp->strict)
            Jsi_LogWarn("Target is not object: %d", target->vt);
        return NULL;
    }
    obj = target->d.obj;
    
#if (defined(JSI_HAS___PROTO__) && JSI_HAS___PROTO__==2)
    if (*key == '_' && Jsi_Strcmp(key, "__proto__")==0 && interp->noproto==0)
        return obj->__proto__;
#endif

    if (*key == 't' && Jsi_Strcmp(key, "this")==0)
        return interp->framePtr->inthis;
    if (obj->arr)
        v = jsi_ObjArrayLookup(interp, obj, key);
    if (!v)
        v= Jsi_TreeObjGetValue(obj, key, isstrkey);
    return v;  /* TODO: redo with copy */
}