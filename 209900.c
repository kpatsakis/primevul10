const char* Jsi_DbKeyAdd(Jsi_Db *jdb, const char *str)
{
#ifndef JSI_LITE_ONLY
    if (jdb->interp)
        return Jsi_KeyAdd(jdb->interp, str);
#endif
    Jsi_HashEntry *hPtr;
    bool isNew;
    hPtr = Jsi_HashEntryNew(jdb->strKeyTbl, str, &isNew);
    assert(hPtr) ;
    return (const char*)Jsi_HashKeyGet(hPtr);
}