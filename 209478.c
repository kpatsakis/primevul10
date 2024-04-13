void jsi_ValueObjSet(Jsi_Interp *interp, Jsi_Value *target, const char *key, Jsi_Value *value, int flags, int isstrkey)
{
    Jsi_TreeEntry *hPtr;
    if (target->vt != JSI_VT_OBJECT) {
        if (interp->strict)
            Jsi_LogWarn("Target is not object: %d", target->vt);
        return;
    }
    hPtr = Jsi_ObjInsert(interp, target->d.obj, key, value, (isstrkey?JSI_OM_ISSTRKEY:0));
    if (!hPtr)
        return;
    hPtr->f.flags |= (flags&JSI_TREE_USERFLAG_MASK);
}