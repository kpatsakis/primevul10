const char* Jsi_KeyLookup(Jsi_Interp *interp, const char *str)
{
    Jsi_MapEntry *hPtr;
    hPtr = Jsi_MapEntryFind(interp->strKeyTbl, str);
    if (!hPtr) {
        return NULL;
    }
    return (const char*)Jsi_MapKeyGet(hPtr, 0);
}