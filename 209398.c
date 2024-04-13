static void IterObjInsert(Jsi_IterObj *io, Jsi_TreeEntry *hPtr)
{
    IterObjInsertKey(io, (const char*)Jsi_TreeKeyGet(hPtr));
}