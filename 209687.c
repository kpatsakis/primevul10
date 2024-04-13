char* jsi_KeyFind(Jsi_Interp *interp, const char *str, int nocreate, int *isKey)
{
    Jsi_MapEntry *hPtr;
    if (isKey) *isKey = 0;
    if (!nocreate) {
        *isKey = 1;
         if (isKey) *isKey = 1;
        return (char*)Jsi_KeyAdd(interp, str);
    }
    hPtr = Jsi_MapEntryFind(interp->strKeyTbl, str);
    if (!hPtr) {
        return Jsi_Strdup(str);;
    }
    if (isKey) *isKey = 1;
    *isKey = 1;
    return (char*)Jsi_MapKeyGet(hPtr, 0);
}