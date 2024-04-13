Jsi_HashEntryNew(Jsi_Hash *tablePtr, const void *key, bool *newPtr)
{
    if (tablePtr->opts.lockHashProc && (*tablePtr->opts.lockHashProc)(tablePtr, 1) != JSI_OK)
        return NULL;
    Jsi_HashEntry *hPtr =  (*((tablePtr)->createProc))(tablePtr, key, newPtr);
#ifdef JSI_HAS_SIG_HASHENTRY
    SIGINIT(hPtr, HASHENTRY);
#endif
    if (tablePtr->opts.lockHashProc)
        (*tablePtr->opts.lockHashProc)(tablePtr, 0);
    return hPtr;
}