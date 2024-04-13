const char* Jsi_KeyAdd(Jsi_Interp *interp, const char *str)
{
    Jsi_MapEntry *hPtr;
    bool isNew;
    hPtr = Jsi_MapEntryNew(interp->strKeyTbl, str, &isNew);
    assert(hPtr) ;
    return (const char*)Jsi_MapKeyGet(hPtr, 0);
}