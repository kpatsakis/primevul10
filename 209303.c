static void jsi_ValueObjDelete(Jsi_Interp *interp, Jsi_Value *target, Jsi_Value *key, int force)
{
    if (target->vt != JSI_VT_OBJECT) return;
    const char *kstr = Jsi_ValueToString(interp, key, NULL);
    Jsi_TreeEntry *hPtr;
    if (!Jsi_ValueIsStringKey(interp, key)) {
        Jsi_MapEntry *hePtr = Jsi_MapEntryFind(target->d.obj->tree->opts.interp->strKeyTbl, kstr);
        if (hePtr)
            kstr = (char*)Jsi_MapKeyGet(hePtr, 0);
    }
    hPtr = Jsi_TreeEntryFind(target->d.obj->tree, kstr);
    if (hPtr == NULL || (hPtr->f.bits.dontdel && !force))
        return;
    Jsi_TreeEntryDelete(hPtr);
}