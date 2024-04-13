Jsi_RC Jsi_TreeKeysDump(Jsi_Interp *interp, Jsi_Tree *tablePtr, Jsi_Value **ret, int flags) {
    char *key;
    int n = 0;
    Jsi_TreeEntry *hPtr;
    Jsi_TreeSearch search;
    Jsi_Obj *nobj;
    Jsi_MapFmtKeyProc* fmtKeyProc = tablePtr->opts.fmtKeyProc;
    
    if (!fmtKeyProc && tablePtr->keyType == JSI_KEYS_ONEWORD && flags!=JSI_KEYS_ONEWORD )
        fmtKeyProc = jsi_treeFmtKey;
        
    if (!fmtKeyProc && tablePtr->keyType >= JSI_KEYS_STRUCT_MINSIZE) 
        return Jsi_LogError("Can not dump struct tree");
    nobj = Jsi_ObjNew(interp);
    Jsi_ValueMakeArrayObject(interp, ret, nobj);
    for (hPtr = Jsi_TreeSearchFirst(tablePtr, &search, flags, NULL);
        hPtr != NULL; hPtr = Jsi_TreeSearchNext(&search)) {
        key = (char*)Jsi_TreeKeyGet(hPtr);
        Jsi_Value *val;
        if (fmtKeyProc) {
            val = (*fmtKeyProc)((Jsi_MapEntry*)hPtr, &tablePtr->opts, flags);
            if (!val) {
                Jsi_LogError("key format failed");
                Jsi_ValueMakeUndef(interp, ret);
                return JSI_ERROR;
            }
        } else if (tablePtr->keyType == JSI_KEYS_ONEWORD)
            val = Jsi_ValueNewNumber(interp, (Jsi_Number)(uintptr_t)key);
        else
            val = Jsi_ValueNewStringKey(interp, key);
        Jsi_ObjArraySet(interp, nobj, val, n++);
        if (fmtKeyProc && val->refCnt>1) //TODO: Hmmm. for StructKey Jsi_OptionsDump() returns refCnt=1
            Jsi_DecrRefCount(interp, val);
    }
    Jsi_TreeSearchDone(&search);
    return JSI_OK;
}