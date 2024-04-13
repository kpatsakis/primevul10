static Jsi_RC jsi_wsFileAdd(Jsi_Interp *interp, jsi_wsCmdObj *cmdPtr, Jsi_Value *name) {
    const char *sname = Jsi_ValueString(interp, name, NULL);
    if (cmdPtr->onModify && sname) {
        bool isNew = 0;
        Jsi_HashEntry *hPtr = Jsi_HashEntryNew(cmdPtr->fileHash, sname, &isNew);
        if (hPtr) {
            jsi_wsFile* fPtr;
            if (!isNew)
                fPtr = Jsi_HashValueGet(hPtr);
            else {
                fPtr = (jsi_wsFile *)Jsi_Calloc(1, sizeof(*fPtr));
                fPtr->fileVal = name;
                fPtr->loadFirst = time(NULL);
                Jsi_IncrRefCount(interp, name);
                fPtr->flags = 0;
                Jsi_HashValueSet(hPtr, fPtr);
            }
            fPtr->loadLast = time(NULL);
        }
    }
    return JSI_OK;
}