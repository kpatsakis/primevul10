static Jsi_RC jsi_DeleteSpecCacheTable(Jsi_Interp *interp, void *clientData)
{
  Jsi_Hash *tablePtr = (Jsi_Hash *) clientData;
  Jsi_HashEntry *entryPtr;
  Jsi_HashSearch search;

  for (entryPtr = Jsi_HashSearchFirst(tablePtr,&search); entryPtr != NULL;
      entryPtr = Jsi_HashSearchNext(&search)) {

    Jsi_Free(Jsi_HashValueGet(entryPtr));
  }
  Jsi_HashDelete(tablePtr);
  return JSI_OK;
}