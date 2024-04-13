Jsi_RC Jsi_HashKeysDump(Jsi_Interp *interp, Jsi_Hash *tablePtr, Jsi_Value **ret, int flags) {
    char *key;
    int n = 0;
    Jsi_HashEntry *hPtr;
    Jsi_HashSearch search;
    Jsi_Obj *nobj;
    Jsi_MapFmtKeyProc* fmtKeyProc = (Jsi_MapFmtKeyProc*)tablePtr->opts.fmtKeyProc;
    
    if (!fmtKeyProc && tablePtr->keyType == JSI_KEYS_ONEWORD && flags!=JSI_KEYS_ONEWORD )
        fmtKeyProc = jsi_hashFmtKey;
    
    if (!fmtKeyProc && tablePtr->keyType >= JSI_KEYS_STRUCT_MINSIZE) 
        return Jsi_LogError("Can not dump struct hash");
    nobj = Jsi_ObjNew(interp);
    Jsi_ValueMakeArrayObject(interp, ret, nobj);
    for (hPtr = Jsi_HashSearchFirst(tablePtr, &search);
        hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
        key = (char*)Jsi_HashKeyGet(hPtr);
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
    return JSI_OK;
}