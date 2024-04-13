Jsi_HashEntryFind( Jsi_Hash *tablePtr, const void *key)
{
    if (tablePtr->opts.lockHashProc && (*tablePtr->opts.lockHashProc)(tablePtr, 1) != JSI_OK)
        return NULL;
    Jsi_HashEntry *hPtr = (*((tablePtr)->findProc))(tablePtr, key);
    if (tablePtr->opts.lockHashProc)
        (*tablePtr->opts.lockHashProc)(tablePtr, 0);
    return hPtr;
}